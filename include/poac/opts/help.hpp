#ifndef POAC_OPTS_HELP_HPP
#define POAC_OPTS_HELP_HPP

#include <future>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

#include <poac/core/except.hpp>
#include <poac/opts/build.hpp>
#include <poac/opts/cache.hpp>
#include <poac/opts/clean.hpp>
#include <poac/opts/graph.hpp>
#include <bin/poac/commands/init.hpp> // FIXME
#include <poac/opts/install.hpp>
#include <bin/poac/commands/new.hpp> // FIXME
#include <poac/opts/run.hpp>
#include <poac/opts/search.hpp>
#include <poac/opts/test.hpp>
#include <poac/opts/uninstall.hpp>
#include <poac/opts/update.hpp>
#include <poac/opts/version.hpp>
#include <poac/util/clap/clap.hpp>

namespace poac::opts::help {
    inline const clap::subcommand cli =
            clap::subcommand("help")
                .about("Prints this message or the help of the given subcommand(s)")
                .arg(clap::arg("subcommand").multiple(true))
            ;

    struct Options {
        enum class Type {
            Summary,
            Usage,
        };
        Type type;
        std::string cmd;
    };

    inline const std::unordered_map<std::string_view, clap::subcommand>
    subcommands{
        { "build",     opts::build::cli },
        { "cache",     opts::cache::cli },
        { "clean",     opts::clean::cli },
        { "graph",     opts::graph::cli },
        { "help",      opts::help::cli },
        { "init",      bin::poac::commands::init::cli },
        { "install",   opts::install::cli },
        { "new",       bin::poac::commands::_new::cli },
        { "run",       opts::run::cli },
        { "search",    opts::search::cli },
        { "test",      opts::test::cli },
        { "uninstall", opts::uninstall::cli },
        { "update",    opts::update::cli },
        { "version",   opts::version::cli },
    };

//    void item(const int n, std::string_view name, std::string_view key) {
//        std::string_view indent = "    ";
//        std::cout << indent << std::left << std::setw(n) << name << summaries_map.at(key) << "\n";
//    }

//    void item(const int n, std::string_view key) {
//        item(n, key, key);
//    }

    void summarize() {
        std::string_view indent = "    ";

        std::cout << "C++'s package manager\n\n"
                  << "USAGE:\n"
                  << indent << "poac [OPTIONS] [SUBCOMMAND]\n\n"
                  << "OPTIONS:\n";

//        item(16, "-v, --version", "version");
//        item(16, "-h, --help", "help");

        std::cout << "\nSome common poac commands are (see all commands with --list):\n";
//        std::vector<std::string> commons = {
//                "build", "clean", "new", "init", "run", "test",
//                "update", "search", "publish", "install", "uninstall"
//        };
//        for (const auto& common : commons) {
//            item(12, common);
//        }
        std::cout << "\nSee `poac help <command>` for information on a specific command." << std::endl;
    }

    void usage(std::string_view s) {
//        std::string_view indent = "    ";

        std::cout << subcommands.at(s);

//        std::cout << "poac-" << s << "\n"
//                  << summaries_map.at(s) << "\n\n"
//                  << "USAGE:\n"
//                  << indent << "poac " << s << " [OPTIONS]\n\n"
//                  << "OPTIONS:\n"
//                  << indent << options_map.at(s) << std::endl;
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
    exec(std::future<std::optional<io::config::Config>>&&, std::vector<std::string>&& args) {
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
