// std
#include <cstdlib>
#include <optional>

// external
#include <boost/algorithm/string.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/cmd.hpp>
#include <poac/util/termcolor2/literals_extra.hpp>

namespace subcmd = poac::cmd;
namespace anyhow = mitama::anyhow;

struct Commands {
    /// Use verbose output
    std::optional<bool> verbose = false;
    /// Do not print poac log messages
    std::optional<bool> quiet = false;

    /// Create a new poac package at <package_name>
    subcmd::create::Options create;

    /// Create a new poac package in an existing directory
    subcmd::init::Options init;
};
STRUCTOPT(subcmd::create::Options, package_name, bin, lib);
STRUCTOPT(subcmd::init::Options, bin, lib);
STRUCTOPT(Commands, verbose, quiet, create, init);

std::string
colorize_error(std::string s) {
    using termcolor2::color_literals::operator""_bold_red;
    boost::replace_all(s, "Error:", "Error:"_bold_red);
    return s;
}

std::string
colorize_help(std::string s) {
    using termcolor2::color_literals::operator""_yellow;
    boost::replace_all(s, "USAGE:", "USAGE:"_yellow);
    boost::replace_all(s, "FLAGS:", "FLAGS:"_yellow);
    boost::replace_all(s, "OPTIONS:", "OPTIONS:"_yellow);
    boost::replace_all(s, "SUBCOMMANDS:", "SUBCOMMANDS:"_yellow);
    return s;
}

std::string
colorize_anyhow_error(std::string s) {
    using termcolor2::color_literals::operator""_yellow;
    boost::replace_all(s, "Caused by:", "Caused by:"_yellow);
    return s;
}

[[nodiscard]] anyhow::result<void>
exec(const structopt::app& app, const Commands& args) {
    if (args.create.has_value()) {
        return subcmd::create::exec(args.create);
    } else if (args.init.has_value()) {
        return subcmd::init::exec(args.init);
    } else {
        spdlog::info("{}", colorize_help(app.help()));
        return mitama::success();
    }
}

int
main(const int argc, char* argv[]) {
    spdlog::set_pattern("%v");
    auto app = structopt::app("poac", POAC_VERSION);

    try {
        const auto args = app.parse<Commands>(argc, argv);

        // Global options
        if (args.verbose.value()) {
            spdlog::set_level(spdlog::level::trace);
        } else if (args.quiet.value()) {
            spdlog::set_level(spdlog::level::off);
        }

        // Subcommands
        using termcolor2::color_literals::operator""_bold_red;
        return exec(app, args)
            .map_err([](const auto& e){
                spdlog::error(
                    "{} {}", "Error:"_bold_red,
                    colorize_anyhow_error(fmt::format("{}", e))
                );
            })
            .is_err();
    } catch (structopt::exception& e) {
        using termcolor2::color_literals::operator""_green;
        spdlog::error(
            "{}\n\nFor more information, try {}",
            colorize_error(e.what()),
            "--help"_green
        );
        return EXIT_FAILURE;
    }
}
