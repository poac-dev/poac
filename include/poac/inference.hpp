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
    // T から const/volatile/reference を除いた型
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
        // idx 番目の型 T について, f.operator()<T>() を実行する
        template <typename F, typename Index>
        static auto apply(F&& f, Index idx) -> decltype(auto) {
            using R = decltype(f.template operator()<at_t<0>>());  // 戻り値の型
            static std::make_index_sequence<size()> seq;  // 整数シーケンス
            return apply<R>(seq, std::forward<F>(f), static_cast<int>(idx));
        }
    private:
        // idx 番目の型 T について, f.operator()<T>() を実行する
        template <typename R, typename F, size_t... Is>
        static R apply(std::index_sequence<Is...>, F&& f, int idx) {
            using func_t = decltype(&apply<R, F, at_t<0>>);  // 関数ポインタの型
            // 関数ポインタテーブルを生成
            // idx 番目の関数ポインタは apply<R, F, at_t<idx>> である
            static func_t func_table[] = { &apply<R, F, at_t<Is>>... };
            return func_table[idx](std::forward<F>(f));
        }
        // 型 T について, f.operator()<T>() を実行する
        template <typename R, typename F, typename T>
        static R apply(F&& f) { return f.template operator()<T>(); }
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
    static const std::unordered_map<std::string, op_type_e> cmdmap {
        { "--help", op_type_e::help },
        { "--version", op_type_e::version },
        { "init", op_type_e::init },
        { "root", op_type_e::root },
        { "new", op_type_e::new_ }
    };
    // TODO: struct作るのではなく，applyをそれぞれ作る方が良い？？？
    struct exec_t {
        template <typename T>
        void operator()() { T()(); }
    };
    struct summary_t {
        template <typename T>
        std::string operator()() { return T::summary(); }
    };
    struct options_t {
        template <typename T>
        std::string operator()() { return T::options(); }
    };

    template <typename T, typename U>
    void _help(const T& key, const U& value) {
        std::cout << "   "
                  << std::setw(9)
                  << std::left
                  << key
                  << "   "
                  << op_type_list_t::apply(summary_t{}, value)
                  << std::endl;
    }
    static void help() { // summary_all
        std::cout << "Usage: poac <command> [<args>]" << std::endl << std::endl
                  << "Available subcommands:" << std::endl;
        bool flag{ false };
        for (const auto& [key, value] : cmdmap) {
            if (key[0] != '-') {
                _help(key, value);
            }
            else if (!flag) {
                flag = !flag;
                std::cout << "Available options:" << std::endl;
                _help(key, value);
            }
            else {
                _help(key, value);
            }
        }
        std::cout << std::endl
                  << "See `poac <command> --help` for information on a specific command." << std::endl
                  << "For full documentation, see: https://github.com/poacpm/poac#readme" << std::endl;
    }
    void exec(const std::string& cmd) {
        if (cmd == "--help" || cmd == "-h")
            help();
        else if (auto itr = cmdmap.find(cmd); itr != cmdmap.end())
            op_type_list_t::apply(exec_t{}, itr->second);
        else
            throw std::invalid_argument("invalid argument");
    }
    std::string summary(const std::string& cmd) {
        if (auto itr = cmdmap.find(cmd); itr != cmdmap.end())
            return op_type_list_t::apply(summary_t{}, itr->second);
        else
            throw std::invalid_argument("invalid argument");
    }
    std::string options(const std::string& cmd) {
        if (auto itr = cmdmap.find(cmd); itr != cmdmap.end())
            return op_type_list_t::apply(options_t{}, itr->second);
        else
            throw std::invalid_argument("invalid argument");
    }
}
#endif