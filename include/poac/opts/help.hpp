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

namespace poac::opts::help {
    const std::string summary = "Display help for a command";
    const std::string options = "<sub-command or option>";

    struct Options {
        enum class Type {
            Summary,
            Usage,
        };
        Type type;
        std::string cmd;
    };

    std::string
    decorate_summary(const std::string& str) {
        return termcolor2::yellow<>.to_string() + str + termcolor2::reset<>.to_string() + "\n";
    }
    std::string
    decorate_name(const std::string& str) {
        // TODO: padding function s -> "build" 9 -> "build    "
        return termcolor2::blue<>.to_string() + termcolor2::bold<>.to_string() +
               "   " + str + "   " + termcolor2::reset<>.to_string();
    }
    std::string
    decorate(const std::string& s1, const std::string& s2) {
        return decorate_name(s1) + decorate_summary(s2);
    }

    std::string
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

    using termcolor2::color_literals::operator""_bold;
    const std::string summary_string =
            "Usage: poac <command> [<args>]\n\n" +
            "Available commands:"_bold + '\n' +
            construct_summary() +
            "\nSee `poac <command> --help` for information on a specific command.\n"
            "For full documentation, see: https://github.com/poacpm/poac#readme";

    // TODO: このmapを用意しているのが無駄．summaryの形成に使用していない
    const std::unordered_map<std::string, std::string>
    summaries_map{
        { "build",     opts::build::summary },
        { "cache",     opts::cache::summary },
        { "cleanup",   opts::cleanup::summary },
        { "graph",     opts::graph::summary },
        { "help",      opts::help::summary },
        { "init",      opts::init::summary },
        { "install",   opts::install::summary },
        { "new",       opts::_new::summary },
        { "publish",   opts::publish::summary },
        { "root",      opts::root::summary },
        { "run",       opts::run::summary },
        { "search",    opts::search::summary },
        { "test",      opts::test::summary },
        { "uninstall", opts::uninstall::summary },
        { "update",    opts::update::summary },
        { "version",   opts::version::summary }
    };

    const std::unordered_map<std::string, std::string>
    options_map{
        { "build",     opts::build::options },
        { "cache",     opts::cache::options },
        { "cleanup",   opts::cleanup::options },
        { "graph",     opts::graph::options },
        { "help",      opts::help::options },
        { "init",      opts::init::options },
        { "install",   opts::install::options },
        { "new",       opts::_new::options },
        { "publish",   opts::publish::options },
        { "root",      opts::root::options },
        { "run",       opts::run::options },
        { "search",    opts::search::options },
        { "test",      opts::test::options },
        { "uninstall", opts::uninstall::options },
        { "update",    opts::update::options },
        { "version",   opts::version::options }
    };

    void usage(const std::string& s) {
        const std::string opt = options_map.at(s);
        const std::string indent = "    ";

        std::cout << "poac-" << s << "\n"
                  << summaries_map.at(s) << "\n\n"
                  << "USAGE:\n"
                  << indent << "poac " << s << " [OPTIONS]\n\n"
                  << "OPTIONS:\n"
                  << indent << opt << std::endl; // TODO: optionの表示をもっとわかりやすくする．cargoのように
    }

    [[nodiscard]] std::optional<core::except::Error>
    help(help::Options&& opts) {
        switch (opts.type) {
            case help::Options::Type::Summary:
                std::cout << summary_string << std::endl;
                return std::nullopt;
            case help::Options::Type::Usage:
                usage(opts.cmd);
                return std::nullopt;
        }
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::config::Config>&&, std::vector<std::string>&& args) {
        help::Options opts{};
        if (args.size() == 0) {
            opts.type = help::Options::Type::Summary;
        } else if (args.size() == 1) {
            opts.type = help::Options::Type::Usage;
            opts.cmd = args[0];
        } else {
            return core::except::Error::InvalidSecondArg::Help;
        }
        return help::help(std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_HELP_HPP
