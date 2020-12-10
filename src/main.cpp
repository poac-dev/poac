// std
#include <cstdlib>
#include <iostream>
#include <string>

// external
#include <clipp.h>
#include <fmt/ostream.h>
#include <plog/Log.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Appenders/ConsoleAppender.h>

// internal
#include <poac/cmd.hpp>

enum class subcommand {
    nothing,
    build,
    init,
    _new,
    search,
    help,
    version,
};

inline const std::string error =
    (termcolor2::bold + termcolor2::red + "Error" + termcolor2::reset).to_string();

[[nodiscard]] int
no_such_command(const int& argc, char* argv[], const clipp::group& cli) {
    fmt::print(
        std::cerr,
        "{}: no such command: `{}`\n\n{}\n",
        error,
        fmt::join(argv + 1, argv + argc," "),
        clipp::usage_lines(cli, "poac")
    );
    return EXIT_FAILURE;
}

void
print_err(std::string_view e) {
    fmt::print(std::cerr, "{}: {}\n", error, e);
}

int
main(const int argc, char* argv[]) {
    static plog::ConsoleAppender<plog::MessageOnlyFormatter> consoleAppender;
    plog::init(plog::info, &consoleAppender);
    const auto set_verbose = []{ plog::get()->setMaxSeverity(plog::verbose); };
    const auto set_quiet = []{ plog::get()->setMaxSeverity(plog::none); };

    subcommand subcmd = subcommand::nothing;

    auto build_opts = poac::cmd::build::Options {
        poac::core::builder::Mode::Debug,
    };
    const clipp::group build_cmd =
        ( clipp::command("build")
            .set(subcmd, subcommand::build)
            .doc("Compile a project and all sources that depend on its")
        , ( clipp::option("--debug", "-d")
                .set(build_opts.mode, poac::core::builder::Mode::Debug)
                .doc("Build artifacts in debug mode [default]")
          | clipp::option("--release", "-r")
                .set(build_opts.mode, poac::core::builder::Mode::Release)
                .doc("Build artifacts in release mode, with optimizations")
          )
        , ( clipp::option("--verbose", "-v")
                .call(set_verbose)
                .doc("Use verbose output")
          | clipp::option("--quiet", "-q")
                .call(set_quiet)
                .doc("No output printed to stdout")
          )
        );

    auto init_opts = poac::cmd::init::Options {
        poac::cmd::_new::ProjectType::Bin,
    };
    const clipp::group init_cmd =
        ( clipp::command("init")
             .set(subcmd, subcommand::init)
             .doc("Create a new poac package in an existing directory")
        , ( clipp::option("--bin", "-b")
                .set(init_opts.type, poac::cmd::_new::ProjectType::Bin)
                .doc("Use a binary (application) template [default]")
          | clipp::option("--lib", "-l")
                .set(init_opts.type, poac::cmd::_new::ProjectType::Lib)
                .doc("Use a library template")
          )
        , ( clipp::option("--verbose", "-v")
                .call(set_verbose)
                .doc("Use verbose output")
          | clipp::option("--quiet", "-q")
                .call(set_quiet)
                .doc("No output printed to stdout")
          )
        );

    auto new_opts = poac::cmd::_new::Options {
        poac::cmd::_new::ProjectType::Bin,
        ""
    };
    const clipp::group new_cmd =
        ( clipp::command("new")
            .set(subcmd, subcommand::_new)
            .doc("Create a new poac package at <path>")
        , clipp::word("path", new_opts.package_name)
        , ( clipp::option("--bin", "-b")
                .set(new_opts.type, poac::cmd::_new::ProjectType::Bin)
                .doc("Use a binary (application) template [default]")
          | clipp::option("--lib", "-l")
                .set(new_opts.type, poac::cmd::_new::ProjectType::Lib)
                .doc("Use a library template")
          )
        , ( clipp::option("--verbose", "-v")
                .call(set_verbose)
                .doc("Use verbose output")
          | clipp::option("--quiet", "-q")
                .call(set_quiet)
                .doc("No output printed to stdout")
          )
        );

    auto search_opts = poac::cmd::search::Options { "" };
    const clipp::group search_cmd =
        ( clipp::command("search")
            .set(subcmd, subcommand::search)
            .doc("Search for packages in poac.pm")
        , clipp::word("pkg-name", search_opts.package_name)
        , ( clipp::option("--verbose", "-v")
              .call(set_verbose)
              .doc("Use verbose output")
          | clipp::option("--quiet", "-q")
              .call(set_quiet)
              .doc("No output printed to stdout")
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
        ) |
        ( build_cmd
        | init_cmd
        | new_cmd
        | search_cmd
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
            case subcommand::build:
                return poac::cmd::build::exec(std::move(build_opts))
                    .map_err(print_err)
                    .is_err();
            case subcommand::init:
                return poac::cmd::init::exec(std::move(init_opts))
                    .map_err(print_err)
                    .is_err();
            case subcommand::_new:
                return poac::cmd::_new::exec(std::move(new_opts))
                    .map_err(print_err)
                    .is_err();
            case subcommand::search:
                return poac::cmd::search::exec(std::move(search_opts))
                    .map_err(print_err)
                    .is_err();
            case subcommand::help:
                std::cout << clipp::make_man_page(cli, "poac");
                return EXIT_SUCCESS;
            case subcommand::version:
                return poac::cmd::version::exec()
                    .map_err(print_err)
                    .is_err();
        }
    } else {
        return no_such_command(argc, argv, cli);
    }
}
