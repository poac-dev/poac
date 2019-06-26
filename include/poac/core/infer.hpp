#ifndef POAC_CORE_INFER_HPP
#define POAC_CORE_INFER_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <type_traits>
#include <utility>

#include <boost/predef.h>

#include "except.hpp"
#include "../opts.hpp"
#include "../util/types.hpp"

namespace poac::core::infer {
    using namespace util::types;

    // Index of T
    // variable template partial specialization
    template <int I, typename T, typename T0, typename... Ts>
    static constexpr int index_of_v = non_type_conditional_v<std::is_same_v<T, T0>, I, index_of_v<I+1, T, Ts...>>;
    template <int I, typename T, typename T0>
    static constexpr int index_of_v<I, T, T0> = non_type_conditional_v<std::is_same_v<T, T0>, I, -1>;

    // type in the index of I
    template <std::size_t I, typename T0, typename... Ts>
    struct at_impl { using type = std::conditional_t<I==0, T0, typename at_impl<I-1, Ts...>::type>; };
    template <std::size_t I, typename T0>
    struct at_impl<I, T0> { using type = std::conditional_t<I==0, T0, void>; };
    template <std::size_t I, typename... Ts>
    using at_impl_t = typename at_impl<I, Ts...>::type;

    // type list
    template <typename... Ts>
    struct type_list_t {
        static constexpr std::size_t size() noexcept { return sizeof...(Ts); };
        template <typename T>
        static constexpr int index_of = index_of_v<0, remove_cvref_t<T>, remove_cvref_t<Ts>...>;
        template <int I>
        using at_t = at_impl_t<I, Ts...>;
    };

    using op_type_list_t = type_list_t<
            opts::build,
            opts::cache,
            opts::cleanup,
            opts::graph,
            opts::help,
            opts::init,
            opts::install,
            opts::new_,
            opts::publish,
            opts::root,
            opts::run,
            opts::search,
            opts::test,
            opts::uninstall,
            opts::update,
            opts::version
    >;
    const std::unordered_map<std::string, int> opts_map {
            { "build",     op_type_list_t::index_of<opts::build> },
            { "cache",     op_type_list_t::index_of<opts::cache> },
            { "cleanup",   op_type_list_t::index_of<opts::cleanup> },
            { "graph",     op_type_list_t::index_of<opts::graph> },
            { "help",      op_type_list_t::index_of<opts::help> },
            { "--help",    op_type_list_t::index_of<opts::help> },
            { "-h",        op_type_list_t::index_of<opts::help> },
            { "init",      op_type_list_t::index_of<opts::init> },
            { "install",   op_type_list_t::index_of<opts::install> },
            { "new",       op_type_list_t::index_of<opts::new_> },
            { "publish",   op_type_list_t::index_of<opts::publish> },
            { "root",      op_type_list_t::index_of<opts::root> },
            { "run",       op_type_list_t::index_of<opts::run> },
            { "search",    op_type_list_t::index_of<opts::search> },
            { "test",      op_type_list_t::index_of<opts::test> },
            { "uninstall", op_type_list_t::index_of<opts::uninstall> },
            { "update",    op_type_list_t::index_of<opts::update> },
            { "version",   op_type_list_t::index_of<opts::version> },
            { "--version", op_type_list_t::index_of<opts::version> },
            { "-v",        op_type_list_t::index_of<opts::version> }
    };

// GCC bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
#if BOOST_COMP_GNUC
    template <typename T, typename VS>
    auto execute2(VS&& vs) {
        return std::to_string(T()(std::forward<VS>(vs)));
    }
    template <std::size_t... Is, typename VS>
    auto execute(std::index_sequence<Is...>, int idx, VS&& vs) {
        using func_t = decltype(&execute2<op_type_list_t::at_t<0>, VS>);
        static func_t func_table[] = { &execute2<op_type_list_t::at_t<Is>>... };
        return func_table[idx](std::forward<VS>(vs));
    }
#else
    // Create function pointer table: { &func<0>, &func<1>, ... }
    // Execute function: &func<idx>[idx]()
    template <std::size_t... Is, typename VS>
    auto execute(std::index_sequence<Is...>, int idx, VS&& vs) {
        // Return "0" or "1" (exit status) because match the type to the other two functions.
        return std::vector({ +[](VS&& vs){
            return std::to_string(op_type_list_t::at_t<Is>()(std::forward<VS>(vs)));
        }... })[idx](std::forward<VS>(vs));
    }
#endif

    template <typename S, typename VS,
              typename Indices=std::make_index_sequence<op_type_list_t::size()>>
    std::string execute(S&& cmd, VS&& arg) {
        namespace except = core::except;
        if (auto itr = opts_map.find(cmd); itr != opts_map.end()) {
            return execute(Indices(), static_cast<int>(itr->second), std::forward<VS>(arg));
        } else {
            throw except::invalid_first_arg("Invalid argument");
        }
    }
}
#endif // !POAC_CORE_INFER_HPP
