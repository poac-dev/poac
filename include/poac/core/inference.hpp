// 文字列から推論してくれる優しいやつ
#ifndef POAC_CORE_INFERENCE_HPP
#define POAC_CORE_INFERENCE_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <type_traits>

#include <boost/predef.h>

#include "except.hpp"
#include "../option.hpp"
#include "../subcmd.hpp"


namespace poac::core::inference {
    // If the type T is a reference type, provides the member typedef type
    //  which is the type referred to by T with its topmost cv-qualifiers removed.
    // Otherwise type is T with its topmost cv-qualifiers removed.
    // C++20, std::remove_cvref_t<T>
    template <typename T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

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

    // type in the index of I
    template <size_t I, typename T0, typename... Ts>
    struct at_impl { using type = std::conditional_t<I==0, T0, typename at_impl<I-1, Ts...>::type>; };
    template <size_t I, typename T0>
    struct at_impl<I, T0> { using type = std::conditional_t<I==0, T0, void>; };
    template <size_t I, typename... Ts>
    using at_impl_t = typename at_impl<I, Ts...>::type;

    // std::initializer_list -> std::vector
    template <typename T>
    static constexpr std::vector<T> make_array( std::initializer_list<T>&& l ) { return l; }

    // type list
    template <typename... Ts>
    struct type_list_t {
        static constexpr size_t size() noexcept { return sizeof...(Ts); };
        template <typename T>
        static constexpr int index_of = index_of_v<0, remove_cvref_t<T>, remove_cvref_t<Ts>...>;
        template <int I>
        using at_t = at_impl_t<I, Ts...>;
    };

    using op_type_list_t = type_list_t<
            subcmd::build,
            subcmd::cache,
            subcmd::init,
            subcmd::install,
            subcmd::login,
            subcmd::new_,
            subcmd::publish,
            subcmd::root,
            subcmd::run,
            subcmd::search,
            subcmd::uninstall,
            option::help,
            option::version
    >;
    enum class op_type_e : int {
        build     = op_type_list_t::index_of<subcmd::build>,
        cache     = op_type_list_t::index_of<subcmd::cache>,
        init      = op_type_list_t::index_of<subcmd::init>,
        install   = op_type_list_t::index_of<subcmd::install>,
        login     = op_type_list_t::index_of<subcmd::login>,
        new_      = op_type_list_t::index_of<subcmd::new_>,
        publish   = op_type_list_t::index_of<subcmd::publish>,
        root      = op_type_list_t::index_of<subcmd::root>,
        run       = op_type_list_t::index_of<subcmd::run>,
        search    = op_type_list_t::index_of<subcmd::search>,
        uninstall = op_type_list_t::index_of<subcmd::uninstall>,
        help      = op_type_list_t::index_of<option::help>,
        version   = op_type_list_t::index_of<option::version>
    };
    const std::unordered_map<std::string, op_type_e> subcmd_map {
            { "build",     op_type_e::build },
            { "cache",     op_type_e::cache },
            { "init",      op_type_e::init },
            { "install",   op_type_e::install },
            { "login",     op_type_e::login },
            { "new",       op_type_e::new_ },
            { "publish",   op_type_e::publish },
            { "root",      op_type_e::root },
            { "run",       op_type_e::run },
            { "search",    op_type_e::search },
            { "uninstall", op_type_e::uninstall }
    };
    const std::unordered_map<std::string, op_type_e> option_map {
            { "--help",    op_type_e::help },
            { "-h",        op_type_e::help },
            { "--version", op_type_e::version },
            { "-v",        op_type_e::version }
    };

// GCC bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
#if BOOST_COMP_GNUC
    template <typename T, typename VS>
    static auto execute2(VS&& vs) { return (T()(vs), ""); }
    template <typename T>
    static auto summary2() { return T::summary(); }
    template <typename T>
    static auto options2() { return T::options(); }
    template <size_t... Is, typename VS>
    static auto execute(std::index_sequence<Is...>, int idx, VS&& vs) {
        using func_t = decltype(&execute2<op_type_list_t::at_t<0>, VS>);
        static func_t func_table[] = { &execute2<op_type_list_t::at_t<Is>>... };
        return func_table[idx](vs);
    }
    template <size_t... Is>
    static auto summary(std::index_sequence<Is...>, int idx) {
        using func_t = decltype(&summary2<op_type_list_t::at_t<0>>);
        static func_t func_table[] = { &summary2<op_type_list_t::at_t<Is>>... };
        return func_table[idx]();
    }
    template <size_t... Is>
    static auto options(std::index_sequence<Is...>, int idx) {
        using func_t = decltype(&options2<op_type_list_t::at_t<0>>);
        static func_t func_table[] = { &options2<op_type_list_t::at_t<Is>>... };
        return func_table[idx]();
    }
#else
    // Create function pointer table: { &func<0>, &func<1>, ... }
    // Execute function: &func<idx>[idx]()
    template <size_t... Is, typename VS>
    static auto execute(std::index_sequence<Is...>, int idx, VS&& vs) {
        // Return ""(empty string) because match the type to the other two functions.
        return make_array({ +[](VS&& vs){ return (op_type_list_t::at_t<Is>()(vs), ""); }... })[idx](vs);
    }
    template <size_t... Is>
    static auto summary(std::index_sequence<Is...>, int idx) {
        return make_array({ +[]{ return op_type_list_t::at_t<Is>::summary(); }... })[idx]();
    }
    template <size_t... Is>
    static auto options(std::index_sequence<Is...>, int idx) {
        return make_array({ +[]{ return op_type_list_t::at_t<Is>::options(); }... })[idx]();
    }
#endif

    // Execute function: execute or summary or options
    template <typename S, typename Index, typename VS, typename Indices=std::make_index_sequence<op_type_list_t::size()>>
    static auto branch(S&& s, Index idx, VS&& vs) -> decltype(summary(Indices(), static_cast<int>(idx))) {
        namespace except = core::except;
        if (s == "exec")
            return execute(Indices(), static_cast<int>(idx), vs);
        else if (s == "summary")
            return summary(Indices(), static_cast<int>(idx));
        else if (s == "options")
            return options(Indices(), static_cast<int>(idx));
        else
            throw except::invalid_first_arg("Invalid argument");
    }

    auto _apply(std::string&& func, const op_type_e& cmd, std::vector<std::string>&& arg) {
        return branch(std::move(func), cmd, std::move(arg));
    }
    std::string apply(std::string&& func, const std::string& cmd, std::vector<std::string>&& arg) {
        namespace except = core::except;
        if (auto itr = subcmd_map.find(cmd); itr != subcmd_map.end())
            return _apply(std::move(func), itr->second, std::move(arg));
        else if (itr = option_map.find(cmd); itr != option_map.end())
            return _apply(std::move(func), itr->second, std::move(arg));
        else
            throw except::invalid_first_arg("Invalid argument");
    }
}
#endif // !POAC_CORE_INFERENCE_HPP
