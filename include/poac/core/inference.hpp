#ifndef POAC_CORE_INFERENCE_HPP
#define POAC_CORE_INFERENCE_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <type_traits>
#include <utility>

#include <boost/predef.h>

#include "./exception.hpp"
#include "../option.hpp"
#include "../subcmd.hpp"
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
            subcmd::build,
            subcmd::cache,
            subcmd::cleanup,
            subcmd::graph,
            subcmd::init,
            subcmd::install,
            subcmd::login,
            subcmd::new_,
            subcmd::publish,
            subcmd::root,
            subcmd::run,
            subcmd::search,
            subcmd::test,
            subcmd::uninstall,
            subcmd::update,
            option::help,
            option::version
    >;
    const std::unordered_map<std::string, int> subcmd_map {
            { "build",     op_type_list_t::index_of<subcmd::build> },
            { "cache",     op_type_list_t::index_of<subcmd::cache> },
            { "cleanup",   op_type_list_t::index_of<subcmd::cleanup> },
            { "graph",     op_type_list_t::index_of<subcmd::graph> },
            { "init",      op_type_list_t::index_of<subcmd::init> },
            { "install",   op_type_list_t::index_of<subcmd::install> },
            { "login",     op_type_list_t::index_of<subcmd::login> },
            { "new",       op_type_list_t::index_of<subcmd::new_> },
            { "publish",   op_type_list_t::index_of<subcmd::publish> },
            { "root",      op_type_list_t::index_of<subcmd::root> },
            { "run",       op_type_list_t::index_of<subcmd::run> },
            { "search",    op_type_list_t::index_of<subcmd::search> },
            { "test",      op_type_list_t::index_of<subcmd::test> },
            { "uninstall", op_type_list_t::index_of<subcmd::uninstall> },
            { "update",    op_type_list_t::index_of<subcmd::update> }
    };
    const std::unordered_map<std::string, int> option_map {
            { "--help",    op_type_list_t::index_of<option::help> },
            { "-h",        op_type_list_t::index_of<option::help> },
            { "--version", op_type_list_t::index_of<option::version> },
            { "-v",        op_type_list_t::index_of<option::version> }
    };

// GCC bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
#if BOOST_COMP_GNUC
    template <typename T, typename VS>
    static auto execute2(VS&& vs) { return (T()(std::forward<VS>(vs)), ""); }
    template <typename T>
    static auto summary2() { return T::summary(); }
    template <typename T>
    static auto options2() { return T::options(); }
    template <std::size_t... Is, typename VS>
    static auto execute(std::index_sequence<Is...>, int idx, VS&& vs) {
        using func_t = decltype(&execute2<op_type_list_t::at_t<0>, VS>);
        static func_t func_table[] = { &execute2<op_type_list_t::at_t<Is>>... };
        return func_table[idx](std::forward<VS>(vs));
    }
    template <std::size_t... Is>
    static auto summary(std::index_sequence<Is...>, int idx) {
        using func_t = decltype(&summary2<op_type_list_t::at_t<0>>);
        static func_t func_table[] = { &summary2<op_type_list_t::at_t<Is>>... };
        return func_table[idx]();
    }
    template <std::size_t... Is>
    static auto options(std::index_sequence<Is...>, int idx) {
        using func_t = decltype(&options2<op_type_list_t::at_t<0>>);
        static func_t func_table[] = { &options2<op_type_list_t::at_t<Is>>... };
        return func_table[idx]();
    }
#else
    // Create function pointer table: { &func<0>, &func<1>, ... }
    // Execute function: &func<idx>[idx]()
    template <std::size_t... Is, typename VS>
    static auto execute(std::index_sequence<Is...>, int idx, VS&& vs) {
        // Return ""(empty string) because match the type to the other two functions.
        return std::vector({ +[](VS&& vs){
            return std::to_string(op_type_list_t::at_t<Is>()(std::forward<VS>(vs)));
        }... })[idx](std::forward<VS>(vs));
    }
    template <std::size_t... Is>
    static auto summary(std::index_sequence<Is...>, int idx) {
        return std::vector({ +[]{ return op_type_list_t::at_t<Is>::summary(); }... })[idx]();
    }
    template <std::size_t... Is>
    static auto options(std::index_sequence<Is...>, int idx) {
        return std::vector({ +[]{ return op_type_list_t::at_t<Is>::options(); }... })[idx]();
    }
#endif

    // Execute function: execute or summary or options
    template <typename S, typename Index, typename VS,
              typename Indices=std::make_index_sequence<op_type_list_t::size()>>
    static auto branch(S&& s, Index idx, VS&& vs) -> decltype(auto) {
        namespace exception = core::exception;
        if (s == "exec")
            return execute(Indices(), static_cast<int>(idx), std::forward<VS>(vs));
        else if (s == "summary")
            return summary(Indices(), static_cast<int>(idx));
        else if (s == "options")
            return options(Indices(), static_cast<int>(idx));
        else
            throw exception::invalid_first_arg("Invalid argument");
    }

    template <typename S, typename OpTypeE, typename VS>
    auto _apply(S&& func, const OpTypeE& cmd, VS&& arg) {
        return branch(std::forward<S>(func), cmd, std::forward<VS>(arg));
    }
    template <typename S, typename VS>
    std::string apply(S&& func, S&& cmd, VS&& arg) {
        namespace exception = core::exception;
        if (auto itr = subcmd_map.find(cmd); itr != subcmd_map.end())
            return _apply(std::forward<S>(func), itr->second, std::forward<VS>(arg));
        else if (itr = option_map.find(cmd); itr != option_map.end())
            return _apply(std::forward<S>(func), itr->second, std::forward<VS>(arg));
        else
            throw exception::invalid_first_arg("Invalid argument");
    }
}
#endif // !POAC_CORE_INFERENCE_HPP
