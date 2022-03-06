// std
#include <cstdlib>
#include <optional>

// external
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

[[nodiscard]] anyhow::result<void>
exec(const structopt::app& app, const Commands& args) {
    if (args.create.has_value()) {
        return subcmd::create::exec(args.create);
    } else if (args.init.has_value()) {
        return subcmd::init::exec(args.init);
    } else {
        return mitama::failure(anyhow::anyhow(app.help()));
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
                spdlog::error("{}: {}", "Error"_bold_red, e);
            })
            .is_err();
    } catch (structopt::exception& e) {
        spdlog::error("{}\n{}", e.what(), e.help());
        return EXIT_FAILURE;
    }
}
