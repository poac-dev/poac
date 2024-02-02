#include "Cli.hpp"
#include "Cmd/Cmd.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"
#include "TermColor.hpp"

#include <cstdlib>
#include <exception>
#include <span>

const Command&
getCmd() noexcept {
  static const Command CMD = //
      Command{ "poac" }
          .setDesc("A package manager and build system for C++")
          .addOpt(Opt{ "--verbose" }
                      .setShort("-v")
                      .setDesc("Use verbose output")
                      .setGlobal(true))
          .addOpt(Opt{ "--quiet" }
                      .setShort("-q")
                      .setDesc("Do not print poac log messages")
                      .setGlobal(true))
          .addOpt(Opt{ "--color" }
                      .setDesc("Coloring: auto, always, never")
                      .setPlaceholder("<WHEN>")
                      .setGlobal(true))
          .addOpt(Opt{ "--help" } //
                      .setShort("-h")
                      .setDesc("Print help")
                      .setGlobal(true))
          .addOpt(Opt{ "--version" }
                      .setShort("-V")
                      .setDesc("Print version info and exit")
                      .setGlobal(false))
          .addOpt(Opt{ "--list" } //
                      .setDesc("List all subcommands")
                      .setGlobal(false)
                      .setHidden(true))
          .addSubcmd(BUILD_CMD)
          .addSubcmd(CLEAN_CMD)
          .addSubcmd(FMT_CMD)
          .addSubcmd(HELP_CMD)
          .addSubcmd(INIT_CMD)
          .addSubcmd(LINT_CMD)
          .addSubcmd(NEW_CMD)
          .addSubcmd(RUN_CMD)
          .addSubcmd(SEARCH_CMD)
          .addSubcmd(TEST_CMD)
          .addSubcmd(TIDY_CMD)
          .addSubcmd(VERSION_CMD);
  return CMD;
}

int
main(int argc, char* argv[]) {
  // Parse arguments (options should appear before the subcommand, as the help
  // message shows intuitively)
  // poac --verbose run --release help --color always --verbose
  // ^^^^^^^^^^^^^^ ^^^^^^^^^^^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // [global]       [run]         [help (under run)]
  const std::span<char* const> args(argv + 1, argv + argc);
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];

    // Global options (which are not command-specific)
    HANDLE_GLOBAL_OPTS({})

    // Local options
    else if (arg == "-V" || arg == "--version") {
      const Vec<StringRef> remArgs(argv + i + 2, argv + argc);
      return versionMain(remArgs);
    }
    else if (arg == "--list") {
      getCmd().printAllSubcmds(true);
      return EXIT_SUCCESS;
    }

    // Subcommands
    else if (getCmd().hasSubcmd(arg)) {
      try {
        // i points to the subcommand name that we don't need anymore.  Since
        // i starts from 1 from the start pointer of argv, we want to start
        // with i + 2.  As we know args.size() + 1 == argc and args.size() >=
        // i + 1, we can write the range as [i + 2, argc), which is never
        // out-of-range access.
        const Vec<StringRef> remArgs(argv + i + 2, argv + argc);
        const int exitCode = getCmd().exec(arg, remArgs);
        if (exitCode != EXIT_SUCCESS) {
          log::error(
              "'poac ", arg, "' failed with exit code `", exitCode, '`'
          );
        }
        return exitCode;
      } catch (const std::exception& e) {
        log::error(e.what());
        return EXIT_FAILURE;
      }
    }

    else {
      return getCmd().noSuchArg(arg);
    }
  }

  return getCmd().printHelp({});
}
