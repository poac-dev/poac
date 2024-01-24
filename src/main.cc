#include "Algos.hpp"
#include "Cmd/Cmd.hpp"
#include "Cmd/Global.hpp"
#include "Cmd/Help.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"
#include "TermColor.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <span>

static const Command&
getCmd() noexcept {
  static const Command CMD =
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
          .addOpt(Opt{ "--help" }
                      .setShort("-h")
                      .setDesc("Print help")
                      .setGlobal(true))
          .addOpt(Opt{ "--version" }
                      .setShort("-V")
                      .setDesc("Print version info and exit")
                      .setGlobal(false))
          .addSubcmd(buildCmd)
          .addSubcmd(cleanCmd)
          .addSubcmd(fmtCmd)
          .addSubcmd(helpCmd)
          .addSubcmd(initCmd)
          .addSubcmd(lintCmd)
          .addSubcmd(newCmd)
          .addSubcmd(runCmd)
          .addSubcmd(searchCmd)
          .addSubcmd(testCmd)
          .addSubcmd(tidyCmd)
          .addSubcmd(versionCmd);
  return CMD;
}

// TODO: Should be in Global.  Also, remove Cmd/Help.  It should be
// automatically generated.
int
helpMain(const std::span<const StringRef> args) noexcept {
  // Parse args
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "help" } })

    else if (getCmd().hasSubcmd(arg)) {
      getCmd().subcmds.at(arg).printHelp();
      return EXIT_SUCCESS;
    }
    else {
      // TODO: should take a list of commands
      return helpCmd.noSuchArg(arg);
    }
  }

  // Print help message for poac itself
  std::cout << "A package manager and build system for C++" << '\n';
  std::cout << '\n';
  printUsage("", cyan("[COMMAND]"));
  std::cout << '\n';

  const usize maxOffset = GLOBAL_OPTS[2].leftSize(2); // --color
  printHeader("Options:");
  printGlobalOpts(2, maxOffset);
  Opt{ "--version" }
      .setShort("-V")
      .setDesc("Print version info and exit")
      .print(2, maxOffset);
  std::cout << '\n';

  printHeader("Commands:");
  for (const auto& [name, cmd] : getCmd().subcmds) {
    //     if (cmd.isShort) {
    // TODO: Correctly handle short name. It's possible that we have longer
    // short.
    if (name.size() == 1) {
      continue;
    }
    // TODO: currently, we assume commands aren't longer than options.
    printCommand(name, cmd.getDesc(), cmd.hasShort(), maxOffset);
  }
  return EXIT_SUCCESS;
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

    // Subcommands
    else if (getCmd().hasSubcmd(arg)) {
      try {
        // i points to the subcommand name that we don't need anymore.  Since
        // i starts from 1 from the start pointer of argv, we want to start
        // with i + 2.  As we know args.size() + 1 == argc and args.size() >=
        // i + 1, we can write the range as [i + 2, argc), which is never
        // out-of-range access.
        const Vec<StringRef> remArgs(argv + i + 2, argv + argc);
        return getCmd().exec(arg, remArgs);
      } catch (const std::exception& e) {
        Logger::error(e.what());
        return EXIT_FAILURE;
      }
    }

    else {
      return getCmd().noSuchArg(arg);
    }
  }

  helpMain({});
  return EXIT_SUCCESS;
}
