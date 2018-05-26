// 文字列から推論してくれる優しいやつ
#ifndef POAC_INFERENCE_HPP
#define POAC_INFERENCE_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <type_traits>

#include "option.hpp"
#include "subcmd.hpp"


namespace poac::inference {
    // The type referred by T or T itself if it is not a reference,
    //  with top-level cv-qualifiers removed.
    template <typename T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
    // C++20, std::remove_cvref_t<T>

    // std::conditional for non-type template
    template <auto Value>
    struct value_holder { static constexpr auto value = Value; };
    template <bool B, auto T, auto F>
    using non_type_conditional = std::conditional<B, value_holder<T>, value_holder<F>>;
    template <bool B, auto T, auto F>
    using non_type_conditional_t = std::conditional_t<B, value_holder<T>, value_holder<F>>;
    template <bool B, auto T, auto F>
    static constexpr auto non_type_conditional_v = non_type_conditional_t<B, T, F>::value;

    // Index of T
    // variable template partial specialization
    template <int I, typename T, typename T0, typename... Ts>
    static constexpr int index_of_v = non_type_conditional_v<std::is_same_v<T, T0>, I, index_of_v<I+1, T, Ts...>>;
    template <int I, typename T, typename T0>
    static constexpr int index_of_v<I, T, T0> = non_type_conditional_v<std::is_same_v<T, T0>, I, -1>;

    // T0,...,Ts の中で I 番目の型 -> type
    template <size_t I, typename T0, typename... Ts>
    struct at_impl {
        using type = std::conditional_t<I==0, T0, typename at_impl<I-1, Ts...>::type>;
    };
    // T0,...,Ts の中で I 番目の型
    template <size_t I, typename T0>
    struct at_impl<I, T0> {
        using type = std::conditional_t<I==0, T0, void>;
    };

    // std::initializer_list -> std::vector
    template <typename T>
    static constexpr std::vector<T> make_array( std::initializer_list<T>&& l ) { return l; }

    template <typename... Ts>
    struct type_list_t {
        static constexpr size_t size() noexcept { return sizeof...(Ts); };
        template <typename T>
        static constexpr int index_of = index_of_v<0, remove_cvref_t<T>, remove_cvref_t<Ts>...>;
        template <int I>
        using at_t = typename at_impl<I, Ts...>::type;
    };

    using op_type_list_t = type_list_t<
            poac::option::help,
            poac::option::version,
            poac::subcmd::init,
            poac::subcmd::root,
            poac::subcmd::new_
    >;
    enum class op_type_e : int {
        help    = op_type_list_t::index_of<poac::option::help>,
        version = op_type_list_t::index_of<poac::option::version>,
        init    = op_type_list_t::index_of<poac::subcmd::init>,
        root    = op_type_list_t::index_of<poac::subcmd::root>,
        new_    = op_type_list_t::index_of<poac::subcmd::new_>
    };
    const std::unordered_map<std::string, op_type_e> subcmd_map {
            { "init", op_type_e::init },
            { "root", op_type_e::root },
            { "new", op_type_e::new_ }
    };
    const std::unordered_map<std::string, op_type_e> option_map {
            { "--help", op_type_e::help },
            { "-h", op_type_e::help },
            { "--version", op_type_e::version },
            { "-v", op_type_e::version }
    };

    // Create function pointer table: { &func<0>, &func<1>, ... }
    // Execute function: &func<idx>[idx]()
    template <size_t... Is>
    static auto execute(std::index_sequence<Is...>, int idx) {
        // Return ""(empty string) because match the type to the other two functions.
        return make_array({ +[]{ using T = op_type_list_t::at_t<Is>; return (T()(), ""); }... })[idx]();
    }
    template <size_t... Is>
    static auto summary(std::index_sequence<Is...>, int idx) {
        return make_array({ +[]{ using T = op_type_list_t::at_t<Is>; return T::summary(); }... })[idx]();
    }
    template <size_t... Is>
    static auto options(std::index_sequence<Is...>, int idx) {
        return make_array({ +[]{ using T = op_type_list_t::at_t<Is>; return T::options(); }... })[idx]();
    }
    // Execute function: func2()
    template <typename S, typename Index, typename Indices=std::make_index_sequence<op_type_list_t::size()>>
    static auto hoge(S&& s, Index idx) -> decltype(summary(Indices(), static_cast<int>(idx))) {
        if (s == "exec")
            return execute(Indices(), static_cast<int>(idx));
        else if (s == "summary")
            return summary(Indices(), static_cast<int>(idx));
        else if (s == "options")
            return options(Indices(), static_cast<int>(idx));
        else
            throw std::invalid_argument("invalid argument");
    }

    std::string _apply(std::string&& func, const op_type_e& type) {
        return hoge(std::move(func), type);
    }
    std::string apply(std::string&& func, const std::string& cmd) {
        if (auto itr = subcmd_map.find(cmd); itr != subcmd_map.end())
            return _apply(std::move(func), itr->second);
        else if (auto itr = option_map.find(cmd); itr != option_map.end())
            return _apply(std::move(func), itr->second);
        else
            throw std::invalid_argument("invalid argument");
    }
}
#endif