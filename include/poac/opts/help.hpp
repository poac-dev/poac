#ifndef POAC_OPTS_HELP_HPP
#define POAC_OPTS_HELP_HPP

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <cstdlib>

#include <boost/predef.h>

#include <poac/opts/build.hpp>
#include <poac/opts/cache.hpp>
#include <poac/opts/cleanup.hpp>
#include <poac/opts/graph.hpp>
#include <poac/opts/init.hpp>
#include <poac/opts/install.hpp>
#include <poac/opts/new.hpp>
#include <poac/opts/publish.hpp>
#include <poac/opts/root.hpp>
#include <poac/opts/run.hpp>
#include <poac/opts/search.hpp>
#include <poac/opts/test.hpp>
#include <poac/opts/uninstall.hpp>
#include <poac/opts/update.hpp>
#include <poac/opts/version.hpp>
#include <poac/core/except.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::help {
    constexpr auto summary = termcolor2::make_string("Display help for a command");
    constexpr auto options = termcolor2::make_string("<sub-command or option>");

    template <typename CharT, std::size_t N, typename Traits>
    constexpr auto
    decorate_summary(const termcolor2::basic_string<CharT, N, Traits>& str) {
        return termcolor2::yellow<> + str + termcolor2::reset<> + "\n";
    }
    template <typename CharT, std::size_t N, typename Traits>
    constexpr auto
    decorate_name(const termcolor2::basic_string<CharT, N, Traits>& str) {
        // TODO: padding function s -> "build" 9 -> "build    "
        return termcolor2::blue<> + termcolor2::bold<> + "   " + str + "   " + termcolor2::reset<>;
    }
    template <typename CharT, std::size_t N, typename Traits, std::size_t M>
    constexpr auto
    decorate(const CharT(&s1)[N], const termcolor2::basic_string<CharT, M, Traits>& s2) {
        return decorate_name(termcolor2::basic_string<CharT, N - 1>(s1)) + decorate_summary(s2);
    }

    constexpr auto
    construct_summary() {
        return decorate("build    ", opts::build::summary)
             + decorate("cache    ", opts::cache::summary)
             + decorate("cleanup  ", opts::cleanup::summary)
             + decorate("graph    ", opts::graph::summary)
             + decorate("help     ", opts::help::summary)
             + decorate("init     ", opts::init::summary)
             + decorate("install  ", opts::install::summary)
             + decorate("new      ", opts::_new::summary)
             + decorate("publish  ", opts::publish::summary)
             + decorate("root     ", opts::root::summary)
             + decorate("run      ", opts::run::summary)
             + decorate("search   ", opts::search::summary)
             + decorate("test     ", opts::test::summary)
             + decorate("uninstall", opts::uninstall::summary)
             + decorate("update   ", opts::update::summary)
             + decorate("version  ", opts::version::summary);
    }

#if BOOST_COMP_MSVC
    const
#else
    constexpr
#endif
    auto summary_string =
            termcolor2::make_string("Usage: poac <command> [<args>]\n\n") +
            termcolor2::bold<> + "Available commands:" + termcolor2::reset<> + '\n' +
            construct_summary() +
            "\nSee `poac <command> --help` for information on a specific command.\n"
            "For full documentation, see: https://github.com/poacpm/poac#readme";

    const std::unordered_map<std::string, std::string>
    options_map{
        { "build",     opts::build::options.to_string() },
        { "cache",     opts::cache::options.to_string() },
        { "cleanup",   opts::cleanup::options.to_string() },
        { "graph",     opts::graph::options.to_string() },
        { "help",      opts::help::options.to_string() },
        { "init",      opts::init::options.to_string() },
        { "install",   opts::install::options.to_string() },
        { "new",       opts::_new::options.to_string() },
        { "publish",   opts::publish::options.to_string() },
        { "root",      opts::root::options.to_string() },
        { "run",       opts::run::options.to_string() },
        { "search",    opts::search::options.to_string() },
        { "test",      opts::test::options.to_string() },
        { "uninstall", opts::uninstall::options.to_string() },
        { "update",    opts::update::options.to_string() },
        { "version",   opts::version::options.to_string() }
    };

    void usage(const std::string& s) {
        const std::string opt = options_map.at(s);
        std::cout << "Usage: poac " << s << " " << opt << std::endl;
    }

    std::optional<core::except::Error>
    exec(const std::vector<std::string> &vs) {
        namespace except = core::except;
        if (vs.size() == 0) {
            std::cout << summary_string << std::endl;
            return std::nullopt;
        } else if (vs.size() == 1) {
            usage(vs[0]);
            return std::nullopt;
        } else {
            return except::Error::InvalidSecondArg::Help{};
        }
    }
} // end namespace
#endif // !POAC_OPTS_HELP_HPP
