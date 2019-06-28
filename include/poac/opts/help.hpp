#ifndef POAC_OPTS_HELP_HPP
#define POAC_OPTS_HELP_HPP

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <cstdlib>

#include "build.hpp"
#include "cache.hpp"
#include "cleanup.hpp"
#include "graph.hpp"
#include "init.hpp"
#include "install.hpp"
#include "new.hpp"
#include "publish.hpp"
#include "root.hpp"
#include "run.hpp"
#include "search.hpp"
#include "test.hpp"
#include "uninstall.hpp"
#include "update.hpp"
#include "version.hpp"
#include "../core/except.hpp"
#include "../util/termcolor2.hpp"

namespace poac::opts::help {
    constexpr auto summary = termcolor2::make_string("Display help for a command");
    constexpr auto options = termcolor2::make_string("<sub-command or option>");

    template <typename Str>
    constexpr auto
    decorate_summary(const Str& s) {
        return termcolor2::yellow<> + s + termcolor2::reset<> + '\n';
    }
    template <typename Str>
    constexpr auto
    decorate_name(const Str& s) {
        // TODO: padding function s -> "build" 9 -> "build    "
        return termcolor2::blue<> + termcolor2::bold<> + "   " + s + "   " + termcolor2::reset<>;
    }
    template <typename Op>
    constexpr auto
    decorate(Op&& op) {
        return decorate_name(op.first) + decorate_summary(op.second);
    }

    template <typename... Opts>
    constexpr auto
    construct_summary(Opts&&... opts) {
        return (... + decorate(std::forward<Opts>(opts)));
    }
    constexpr auto
    construct_summary() {
        return construct_summary(
                std::make_pair(termcolor2::make_string("build    "), opts::build::summary),
                std::make_pair(termcolor2::make_string("cache    "), opts::cache::summary),
                std::make_pair(termcolor2::make_string("cleanup  "), opts::cleanup::summary),
                std::make_pair(termcolor2::make_string("graph    "), opts::graph::summary),
                std::make_pair(termcolor2::make_string("help     "), opts::help::summary),
                std::make_pair(termcolor2::make_string("init     "), opts::init::summary),
                std::make_pair(termcolor2::make_string("install  "), opts::install::summary),
                std::make_pair(termcolor2::make_string("new      "), opts::_new::summary),
                std::make_pair(termcolor2::make_string("publish  "), opts::publish::summary),
                std::make_pair(termcolor2::make_string("root     "), opts::root::summary),
                std::make_pair(termcolor2::make_string("run      "), opts::run::summary),
                std::make_pair(termcolor2::make_string("search   "), opts::search::summary),
                std::make_pair(termcolor2::make_string("test     "), opts::test::summary),
                std::make_pair(termcolor2::make_string("uninstall"), opts::uninstall::summary),
                std::make_pair(termcolor2::make_string("update   "), opts::update::summary),
                std::make_pair(termcolor2::make_string("version  "), opts::version::summary)
        );
    }

    constexpr auto summary_string =
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
    _main(const std::vector<std::string>& vs) {
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
