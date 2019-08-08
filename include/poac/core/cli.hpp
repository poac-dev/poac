#ifndef POAC_CORE_CLI_HPP
#define POAC_CORE_CLI_HPP

#include <functional>
#include <future>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include <poac/core/except.hpp>
#include <poac/io/config.hpp>
#include <poac/opts.hpp>
#include <poac/util/clap/clap.hpp>

namespace poac::core::cli {
    std::vector<clap::subcommand> builtin() {
        return {
            opts::build::cli,
            opts::cache::cli,
            opts::clean::cli,
            opts::graph::cli,
            opts::help::cli,
            opts::init::cli,
            opts::install::cli,
            opts::_new::cli,
            opts::publish::cli,
            opts::root::cli,
            opts::run::cli,
            opts::search::cli,
            opts::test::cli,
            opts::uninstall::cli,
            opts::update::cli,
            opts::version::cli,
        };
    }

    const clap::app cli =
            clap::app("poac")
                .version(POAC_VERSION)
                .arg(clap::opt("version", "Print version info and exit").short_("V"))
                .arg(clap::opt("list", "List installed commands"))
                .arg(
                    clap::opt(
                        "verbose",
                        "Use verbose output (-vv very verbose/build.rs output)"
                    )
                    .short_("v")
                    .multiple(true)
                    .global(true)
                )
                .arg(clap::opt("quiet", "No output printed to stdout").short_("q"))
                .subcommands(cli::builtin())
//                .fooks(cli::builtin_exec())
            ;

    using arg1_type = std::future<std::optional<io::config::Config>>;
    using arg2_type = std::vector<std::string>;
    using ret_type = std::optional<except::Error>;
    using fn_type = std::function<ret_type(arg1_type, arg2_type)>;

    const std::unordered_map<std::string_view, fn_type>
    opts_map{
        { "build",     opts::build::exec },
        { "cache",     opts::cache::exec },
        { "clean",     opts::clean::exec },
        { "graph",     opts::graph::exec },
        { "help",      opts::help::exec }, //
        { "--help",    opts::help::exec }, //
        { "-h",        opts::help::exec }, //
        { "init",      opts::init::exec },
        { "install",   opts::install::exec },
        { "new",       opts::_new::exec },
        { "publish",   opts::publish::exec },
        { "root",      opts::root::exec },
        { "run",       opts::run::exec },
        { "search",    opts::search::exec },
        { "test",      opts::test::exec },
        { "uninstall", opts::uninstall::exec },
        { "update",    opts::update::exec },
        { "version",   opts::version::exec }, //
        { "--version", opts::version::exec }, //
        { "-v",        opts::version::exec }, //
    };

    [[nodiscard]] ret_type
    exec(std::string_view cmd, std::vector<std::string>&& args) {
        try {
            opts_map.at(cmd);
        } catch(std::out_of_range&) {
            return except::Error::InvalidFirstArg;
        }
        return opts_map.at(cmd)(
                std::async(std::launch::async, io::config::load, io::path::current),
                std::move(args));
    }
}
#endif // !POAC_CORE_CLI_HPP
