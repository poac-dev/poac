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
            termcolor2::to_red(termcolor2::to_bold(std::string("error"))),
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
    return opt.has_value() ? EXIT_FAILURE : EXIT_SUCCESS;
}

[[nodiscard]] std::optional<poac::core::except::Error>
exec(const subcommand& subcmd, const clipp::group& cli) {
    switch (subcmd) {
        case subcommand::help:
            std::cout << clipp::make_man_page(cli, "poac");
            return std::nullopt;
        case subcommand::version:
            return poac::cmd::version::exec();
        case subcommand::nothing:
            return std::nullopt;
    }
}

int
main(const int argc, char* argv[]) {
    subcommand subcmd = subcommand::nothing;
    option opt = option::nothing;

    const clipp::group cli = (
        clipp::with_prefixes_short_long(
            "-",
            "--",
            clipp::option("V", "version")
                .set(subcmd, subcommand::version)
                .doc("Print version info"),
            clipp::option("v", "verbose")
                .set(opt, option::verbose)
                .doc("Use verbose output"),
            clipp::option("q", "quiet")
                .set(opt, option::quiet)
                .doc("No output printed to stdout")
        ) |
        (
            clipp::command("help").set(subcmd, subcommand::help) |
            clipp::command("version").set(subcmd, subcommand::version)
        )
    );

    if (argc == 1) {
        std::cout << clipp::usage_lines(cli, "poac") << std::endl;
        return EXIT_SUCCESS;
    } else if (clipp::parse(argc, argv, cli)) {
        if (subcmd == subcommand::nothing) {
            return no_such_command(argc, argv, cli);
        } else {
            return optional_to_int(exec(subcmd, cli));
        }
    } else {
        return no_such_command(argc, argv, cli);
    }
}
