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
  static const Command cmd = //
      Command{ "poac" }
          .setDesc("A package manager and build system for C++")
          .addOpt(Opt{ "--verbose" }
                      .setShort("-v")
                      .setDesc("Use verbose output (-vv very verbose output)")
                      .setGlobal(true))
          .addOpt(Opt{ "-vv" }
                      .setDesc("Use very verbose output")
                      .setGlobal(true)
                      .setHidden(true))
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
  return cmd;
}

int
main(int argc, char* argv[]) {
  // Parse arguments (options should appear before the subcommand, as the help
  // message shows intuitively)
  // poac --verbose run --release help --color always --verbose
  // ^^^^^^^^^^^^^^ ^^^^^^^^^^^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  // [global]       [run]         [help (under run)]
  const std::span<char* const> args(argv + 1, argv + argc);
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Command::handleGlobalOpts(itr, args.end(), "")) {
      return res.value();
    }

    // Local options
    else if (*itr == "-V"sv || *itr == "--version"sv) {
      const Vec<StringRef> remArgs(itr + 1, args.end());
      return versionMain(remArgs);
    } else if (*itr == "--list"sv) {
      getCmd().printAllSubcmds(true);
      return EXIT_SUCCESS;
    }

    // Subcommands
    else if (getCmd().hasSubcmd(*itr)) {
      try {
        const Vec<StringRef> remArgs(itr + 1, args.end());
        const int exitCode = getCmd().exec(*itr, remArgs);
        if (exitCode != EXIT_SUCCESS) {
          logger::error(
              "'poac ", *itr, "' failed with exit code `", exitCode, '`'
          );
        }
        return exitCode;
      } catch (const std::exception& e) {
        logger::error(e.what());
        return EXIT_FAILURE;
      }
    }

    else {
      return getCmd().noSuchArg(*itr);
    }
  }

  return getCmd().printHelp({});
}
