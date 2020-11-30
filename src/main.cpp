#include <clipp.h>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/os.h>
#include <iostream>
#include <poac/cmd.hpp>
#include <poac/core/except.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <string>
#include <optional>

enum class subcommand {
    nothing,
    init,
    help,
    version,
};

enum class option {
    nothing,
    verbose,
    quiet,
};

[[nodiscard]] int
no_such_command(const int& argc, char* argv[], const clipp::group& cli) {
    std::cerr
        << fmt::format(
            "{}: no such command: `{}`",
            termcolor2::to_red(termcolor2::to_bold(std::string("Error"))),
            fmt::join(argv + 1, argv + argc," ")
        )
        << std::endl
        << std::endl
        << clipp::usage_lines(cli, "poac")
        << std::endl;
    return EXIT_FAILURE;
}

template <typename T>
int
optional_to_int(const std::optional<T>& opt) noexcept {
    return opt.has_value()
               ? (std::cerr << opt->what() << std::endl, EXIT_FAILURE)
               : EXIT_SUCCESS;
}

int
main(const int argc, char* argv[]) {
    subcommand subcmd = subcommand::nothing;
    option opt = option::nothing;

    auto init_opts = poac::cmd::init::Options {
        poac::cmd::_new::ProjectType::Bin
    };
    const clipp::group init_cmd =
        ( clipp::command("init")
             .set(subcmd, subcommand::init)
             .doc("Create a new poac package in an existing directory")
        , ( clipp::option("--bin", "-b")
                .doc("Use a binary (application) template [default]")
          | clipp::option("--lib", "-l")
                .set(init_opts.type, poac::cmd::_new::ProjectType::Lib)
                .doc("Use a library template")
          )
        );

    const clipp::parameter help_cmd =
        clipp::command("help")
            .set(subcmd, subcommand::help)
            .doc("Print this message");

    const clipp::parameter version_cmd =
        clipp::command("version")
            .set(subcmd, subcommand::version)
            .doc("Show the current poac version");

    const clipp::group cli = (
        ( clipp::option("--help", "-h")
            .set(subcmd, subcommand::help)
            .doc("Print this message or the help of the given subcommand(s)")
        , clipp::option("--version", "-V")
            .set(subcmd, subcommand::version)
            .doc("Show the current poac version")
        , clipp::option("--verbose", "-v")
            .set(opt, option::verbose)
            .doc("Use verbose output")
        , clipp::option("--quiet", "-q")
             .set(opt, option::quiet)
             .doc("No output printed to stdout")
        ) |
        ( init_cmd
        | help_cmd
        | version_cmd
        )
    );

    if (argc == 1) {
        std::cout << clipp::usage_lines(cli, "poac") << std::endl;
        return EXIT_SUCCESS;
    } else if (clipp::parse(argc, argv, cli)) {
        switch (subcmd) {
            case subcommand::nothing:
                return no_such_command(argc, argv, cli);
            case subcommand::init:
                return optional_to_int(poac::cmd::init::exec(std::move(init_opts)));
            case subcommand::help:
                std::cout << clipp::make_man_page(cli, "poac");
                return EXIT_SUCCESS;
            case subcommand::version:
                return optional_to_int(poac::cmd::version::exec());
        }
    } else {
        return no_such_command(argc, argv, cli);
    }
}
