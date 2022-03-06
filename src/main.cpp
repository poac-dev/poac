// std
#include <cstdlib>
#include <optional>

// external
#include <mitama/anyhow/anyhow.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/cmd.hpp>

namespace commands = poac::cmd;
namespace anyhow = mitama::anyhow;

struct Commands {
    /// Use verbose output
    std::optional<bool> verbose = false;
    /// Do not print poac log messages
    std::optional<bool> quiet = false;

    /// Create a new poac package in an existing directory
    commands::init::Options init;
};
STRUCTOPT(commands::init::Options, bin, lib);
STRUCTOPT(Commands, verbose, quiet, init);

[[nodiscard]] anyhow::result<void>
exec(const structopt::app& app, const Commands& args) {
    if (args.init.has_value()) {
        return commands::init::exec(args.init);
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
