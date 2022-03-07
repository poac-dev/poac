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

using namespace termcolor2::color_literals;
namespace subcmd = poac::cmd;
namespace anyhow = mitama::anyhow;

struct Commands {
    /// Use verbose output
    std::optional<bool> verbose = false;
    /// Do not print poac log messages
    std::optional<bool> quiet = false;

    /// Compile a local package and all of its dependencies
    subcmd::build::Options build;

    /// Create a new poac package at <package_name>
    subcmd::create::Options create;

    /// Create a new poac package in an existing directory
    subcmd::init::Options init;

    /// Search a package on poac.pm
    subcmd::search::Options search;
};
STRUCTOPT(Commands, verbose, quiet, build, create, init, search);

inline std::string
colorize_structopt_error(std::string s) {
    boost::replace_all(s, "Error:", "Error:"_bold_red);
    return s;
}

inline std::string
colorize_anyhow_error(std::string s) {
    boost::replace_all(s, "Caused by:", "Caused by:"_yellow);
    return s;
}

inline std::string
colorize_help(std::string s) {
    boost::replace_all(s, "USAGE:", "USAGE:"_yellow);
    boost::replace_all(s, "FLAGS:", "FLAGS:"_yellow);
    boost::replace_all(s, "OPTIONS:", "OPTIONS:"_yellow);
    boost::replace_all(s, "SUBCOMMANDS:", "SUBCOMMANDS:"_yellow);
    return s;
}

[[nodiscard]] anyhow::result<void>
exec(const structopt::app& app, const Commands& args) {
    if (args.build.has_value()) {
        return subcmd::build::exec(args.build);
    } else if (args.create.has_value()) {
        return subcmd::create::exec(args.create);
    } else if (args.init.has_value()) {
        return subcmd::init::exec(args.init);
    } else if (args.search.has_value()) {
        return subcmd::search::exec(args.search);
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
        return exec(app, args)
            .map_err([](const auto& e){
                spdlog::error(
                    "{} {}", "Error:"_bold_red,
                    colorize_anyhow_error(fmt::format("{}", e))
                );
            })
            .is_err();
    } catch (structopt::exception& e) {
        spdlog::error(
            "{}\n\nFor more information, try {}",
            colorize_structopt_error(e.what()),
            "--help"_green
        );
        return EXIT_FAILURE;
    } catch (...) {
        spdlog::error(
            "{} Unknown error occurred\n\n"
            "Please open an issue with reproducible information at:\n"
            "https://github.com/poacpm/poac/issues",
            "Error:"_bold_red
        );
        return EXIT_FAILURE;
    }
}
