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

namespace poac::opts {
    namespace _help {
        constexpr auto summary() {
            return termcolor2::make_string("Display help for a command");
        }
        constexpr auto options() {
            return termcolor2::make_string("<sub-command or option>");
        }

        template <typename Str>
        constexpr auto
        decorate_summary(Str&& s) {
            return termcolor2::yellow<> + s + termcolor2::reset<> + '\n';
        }
        template <typename Str>
        constexpr auto
        decorate_name(Str&& s) {
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
            return (... + decorate(opts));
        }
        constexpr auto
        construct_summary() {
            return construct_summary(
                    std::make_pair(termcolor2::make_string("build    "), opts::_build::summary()),
                    std::make_pair(termcolor2::make_string("cache    "), opts::_cache::summary()),
                    std::make_pair(termcolor2::make_string("cleanup  "), opts::_cleanup::summary()),
                    std::make_pair(termcolor2::make_string("graph    "), opts::_graph::summary()),
                    std::make_pair(termcolor2::make_string("help     "), opts::_help::summary()),
                    std::make_pair(termcolor2::make_string("init     "), opts::_init::summary()),
                    std::make_pair(termcolor2::make_string("install  "), opts::_install::summary()),
                    std::make_pair(termcolor2::make_string("new      "), opts::_new::summary()),
                    std::make_pair(termcolor2::make_string("publish  "), opts::_publish::summary()),
                    std::make_pair(termcolor2::make_string("root     "), opts::_root::summary()),
                    std::make_pair(termcolor2::make_string("run      "), opts::_run::summary()),
                    std::make_pair(termcolor2::make_string("search   "), opts::_search::summary()),
                    std::make_pair(termcolor2::make_string("test     "), opts::_test::summary()),
                    std::make_pair(termcolor2::make_string("uninstall"), opts::_uninstall::summary()),
                    std::make_pair(termcolor2::make_string("update   "), opts::_update::summary()),
                    std::make_pair(termcolor2::make_string("version  "), opts::_version::summary())
            );
        }

        constexpr auto summary_string =
                termcolor2::make_string("Usage: poac <command> [<args>]\n\n") +
                termcolor2::bold<> + "Available commands:" + termcolor2::reset<> + '\n' +
                construct_summary() +
                "\nSee `poac <command> --help` for information on a specific command.\n"
                "For full documentation, see: https://github.com/poacpm/poac#readme";

        std::unordered_map<std::string, std::string>
        options_map({
            { "build", opts::_build::options().to_string() },
            { "cache", opts::_cache::options().to_string() },
            { "cleanup", opts::_cleanup::options().to_string() },
            { "graph", opts::_graph::options().to_string() },
            { "help", opts::_help::options().to_string() },
            { "init", opts::_init::options().to_string() },
            { "install", opts::_install::options().to_string() },
            { "new", opts::_new::options().to_string() },
            { "publish", opts::_publish::options().to_string() },
            { "root", opts::_root::options().to_string() },
            { "run", opts::_run::options().to_string() },
            { "search", opts::_search::options().to_string() },
            { "test", opts::_test::options().to_string() },
            { "uninstall", opts::_uninstall::options().to_string() },
            { "update", opts::_update::options().to_string() },
            { "version", opts::_version::options().to_string() }
        });

        template <typename VS>
        int _main(VS&& vs) {
            namespace except = core::except;
            if (vs.size() == 0) {
                std::cout << summary_string << std::endl;
                return EXIT_SUCCESS;
            } else if (vs.size() == 1) {
                std::cout << "Usage: poac " << vs[0] << " "
                          << options_map[vs[0]]
                          << std::endl;
                return EXIT_SUCCESS;
            } else {
                throw except::invalid_second_arg("--help");
            }
        }
    }

    struct help {
        template <typename VS>
        int operator()(VS&& argv) {
            return _help::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_OPTS_HELP_HPP
