// 文字列から推論してくれる優しいやつ
#ifndef POAC_INFERENCE_HPP
#define POAC_INFERENCE_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <functional>

#include "option.hpp"
#include "subcmd.hpp"


namespace poac::inference {
    // The type referred by T or T itself if it is not a reference,
    //  with top-level cv-qualifiers removed.
    template <typename T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
    // C++20, std::remove_cvref_t<T>

    // T0,...,Ts の中で T が出現する最初の位置 -> size_t
    template <size_t I, typename T, typename T0, typename... Ts>
    struct index_of_impl {
        static const size_t value = std::is_same_v<T, T0> ? I : index_of_impl<I+1, T, Ts...>::value;
    };
    // T0,...,Ts の中で T が出現する最初の位置
    template <size_t I, typename T, typename T0>
    struct index_of_impl<I, T, T0> {
        static const size_t value = std::is_same_v<T, T0> ? I : static_cast<size_t>(-1);
    };

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


    // TODO: こういった大きなクラスに依存しない設計にしたい．
    // 型のリスト
    template <typename... Ts>
    struct type_list_t {
        // 型数
        static constexpr size_t size() noexcept { return sizeof...(Ts); };
        // T が最初に現れる位置
        template <typename T>
        static constexpr int index_of = static_cast<int>(index_of_impl<0, remove_cvref_t<T>, remove_cvref_t<Ts>...>::value);
        // I 番目の型
        template <int I>
        using at_t = typename at_impl<I, Ts...>::type;

        // Create function pointer table: { &func<0>, &func<1>, ... }
        // Execute function: &func<idx>[idx]()
        template <size_t... Is>
        static auto exec2(std::index_sequence<Is...>, int idx) {
            return make_array({ +[]{ using T = at_t<Is>; return T()(); }... })[idx]();
        }
        template <size_t... Is>
        static auto summary2(std::index_sequence<Is...>, int idx) {
            return make_array({ +[]{ using T = at_t<Is>; return T::summary(); }... })[idx]();
        }
        template <size_t... Is>
        static auto options2(std::index_sequence<Is...>, int idx) {
            return make_array({ +[]{ using T = at_t<Is>; return T::options(); }... })[idx]();
        }
        // Execute function: func2()
        template <typename Index, typename Indices=std::make_index_sequence<size()>>
        static auto exec1(Index idx) {
            return exec2(Indices(), static_cast<int>(idx));
        }
        template <typename Index, typename Indices=std::make_index_sequence<size()>>
        static auto summary1(Index idx) {
            return summary2(Indices(), static_cast<int>(idx));
        }
        template <typename Index, typename Indices=std::make_index_sequence<size()>>
        static auto options1(Index idx) {
            return options2(Indices(), static_cast<int>(idx));
        }
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


    // TODO: これらを一つにまとめたい．文字列から推論してほしい???
    void _exec(const op_type_e& type) { return op_type_list_t::exec1(type); }
    void exec(const std::string& cmd) {
        if (auto itr = subcmd_map.find(cmd); itr != subcmd_map.end())
            _exec(itr->second);
        else if (auto itr = option_map.find(cmd); itr != option_map.end())
            _exec(itr->second);
        else
            throw std::invalid_argument("invalid argument");
    }
    std::string _summary(const op_type_e& type) { return op_type_list_t::summary1(type); }
    std::string summary(const std::string& cmd) {
        if (auto itr = subcmd_map.find(cmd); itr != subcmd_map.end())
            return _summary(itr->second);
        else if (auto itr = option_map.find(cmd); itr != option_map.end())
            return _summary(itr->second);
        else
            throw std::invalid_argument("invalid argument");
    }
    std::string _options(const op_type_e& type) { return op_type_list_t::options1(type); }
    std::string options(const std::string& cmd) {
        if (auto itr = subcmd_map.find(cmd); itr != subcmd_map.end())
            return _options(itr->second);
        else if (auto itr = option_map.find(cmd); itr != option_map.end())
            return _options(itr->second);
        else
            throw std::invalid_argument("invalid argument");
    }
}
#endif