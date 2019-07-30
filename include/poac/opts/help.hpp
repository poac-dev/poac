#ifndef POAC_OPTS_HELP_HPP
#define POAC_OPTS_HELP_HPP

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include <poac/core/except.hpp>
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

    void summarize() {
        const std::string indent = "    ";

        std::cout << "C++'s package manager\n\n"
                  << "USAGE:\n"
                  << indent << "poac [OPTIONS] [SUBCOMMAND]\n\n"
                  << "OPTIONS:\n";

        std::cout << indent << std::left << std::setw(16) << "-v, --version" << summaries_map.at("version") << "\n";
        std::cout << indent << std::left << std::setw(16) << "-h, --help" << summaries_map.at("help") << "\n";

        std::cout << "\nSome common poac commands are (see all commands with --list):\n";
        std::vector<std::string> commons = {
                "build", "cleanup", "new", "init", "run", "test",
                "update", "search", "publish", "install", "uninstall"
        };
        for (const auto& common : commons) {
            std::cout << indent << std::left << std::setw(12) << common << summaries_map.at(common) << "\n";
        }
        std::cout << "\nSee `poac help <command>` for information on a specific command." << std::endl;
    }

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

    void help(help::Options&& opts) {
        switch (opts.type) {
            case help::Options::Type::Summary:
                summarize();
                break;
            case help::Options::Type::Usage:
                usage(opts.cmd);
                break;
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
        help::help(std::move(opts));
        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_HELP_HPP
