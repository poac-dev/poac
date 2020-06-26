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

#include <bin/poac/commands.hpp>
#include <poac/core/except.hpp>
#include <poac/io/config.hpp>
#include <poac/io/path.hpp>
#include <poac/opts.hpp>
#include <poac/util/clap/clap.hpp>

namespace bin::poac {
    inline const clap::app cli =
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
                .subcommands(commands::builtin())
//                .fooks(cli::builtin_exec())
            ;

    [[nodiscard]] std::optional<::poac::core::except::Error>
    exec(std::string_view cmd, std::vector<std::string>&& args) {
        if (auto f = commands::builtin_exec(cmd)) {
            return f.value()(std::async(
                    std::launch::async,
                    ::poac::io::config::load,
                    ::poac::io::path::current), std::move(args));
        } else {
            return ::poac::core::except::Error::InvalidFirstArg;
        }
    }
}
#endif // !POAC_CORE_CLI_HPP
