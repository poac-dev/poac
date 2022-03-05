// std
#include <cstdlib>
#include <string>
#include <optional>

// external
#include <mitama/result/result.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/cmd.hpp>

inline TERMCOLOR2_CXX20_CONSTINIT const std::string error =
    termcolor2::bold + termcolor2::red + "Error" + termcolor2::reset;

void
print_err(std::string_view e) {
    spdlog::error("{}: {}", error, e);
}

struct Command {
    /// Use verbose output
    std::optional<bool> verbose = false;
    /// Do not print poac log messages
    std::optional<bool> quiet = false;

    /// Create a new poac package in an existing directory
    poac::cmd::init::Options init;
};
STRUCTOPT(poac::cmd::init::Options, bin, lib);
STRUCTOPT(Command, verbose, quiet, init);

[[nodiscard]] mitama::result<void, std::string>
exec(const structopt::app& app, Command& args) {
    if (args.init.has_value()) {
        return poac::cmd::init::exec(std::move(args.init));
    } else {
        return mitama::failure(app.help());
    }
}

int
main(const int argc, char* argv[]) {
    spdlog::set_pattern("%v");
    auto app = structopt::app("poac", "0.2.1");

    try {
        auto args = app.parse<Command>(argc, argv);

        // Global options
        if (args.verbose.value()) {
            spdlog::set_level(spdlog::level::trace);
        } else if (args.quiet.value()) {
            spdlog::set_level(spdlog::level::off);
        }

        // Subcommands
        return exec(app, args).map_err(print_err).is_err();
    } catch (structopt::exception& e) {
        spdlog::error("{}\n{}", e.what(), e.help());
        return EXIT_FAILURE;
    }
}
