#include "Algos.hpp"
#include "Cmd/Build.hpp"
#include "Cmd/Clean.hpp"
#include "Cmd/Fmt.hpp"
#include "Cmd/Global.hpp"
#include "Cmd/Help.hpp"
#include "Cmd/Init.hpp"
#include "Cmd/Lint.hpp"
#include "Cmd/New.hpp"
#include "Cmd/Run.hpp"
#include "Cmd/Search.hpp"
#include "Cmd/Test.hpp"
#include "Cmd/Tidy.hpp"
#include "Cmd/Version.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"
#include "TermColor.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <span>

struct Cmd {
  const Fn<int(std::span<const StringRef>)> main;
  const Subcmd& cmd;
  const bool isShort = false;
};

#define DEFINE_CMD(name)    \
  {                         \
    #name, {                \
      name##Main, name##Cmd \
    }                       \
  }

#define DEFINE_SHORT_CMD(name)    \
  {                               \
    StringRef(&#name[0], 1), {    \
      name##Main, name##Cmd, true \
    }                             \
  }

#define DEFINE_CMD_WITH_SHORT(name) DEFINE_CMD(name), DEFINE_SHORT_CMD(name)

static const HashMap<StringRef, Cmd>&
getCmds() noexcept {
  static const HashMap<StringRef, Cmd> CMDS = {
    DEFINE_CMD_WITH_SHORT(build),
    DEFINE_CMD(clean),
    DEFINE_CMD(fmt),
    DEFINE_CMD(help),
    DEFINE_CMD(init),
    DEFINE_CMD(lint),
    DEFINE_CMD(new),
    DEFINE_CMD_WITH_SHORT(run),
    DEFINE_CMD(search),
    DEFINE_CMD_WITH_SHORT(test),
    DEFINE_CMD(tidy),
    DEFINE_CMD(version),
  };
  return CMDS;
}

void
noSuchCommand(const StringRef arg) {
  Vec<StringRef> candidates(getCmds().size());
  usize idx = 0;
  for (const auto& cmd : getCmds()) {
    candidates[idx++] = cmd.first;
  }

  String suggestion;
  if (const auto similar = findSimilarStr(arg, candidates)) {
    suggestion = "       Did you mean `" + String(similar.value()) + "`?\n\n";
  }
  Logger::error(
      "no such command: `", arg, "`\n\n", suggestion,
      "       Run `poac help` for a list of commands"
  );
}

int
helpMain(const std::span<const StringRef> args) noexcept {
  // Parse args
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "help" } })

    else if (getCmds().contains(arg)) {
      getCmds().at(arg).cmd.printHelp();
      return EXIT_SUCCESS;
    }
    else {
      noSuchCommand(arg);
      return EXIT_FAILURE;
    }
  }

  // Print help message for poac itself
  std::cout << "A package manager and build system for C++" << '\n';
  std::cout << '\n';
  printUsage("", "[COMMAND]");
  std::cout << '\n';

  const usize maxOffset = GLOBAL_OPTS[2].leftSize() + 2; // --color
  printHeader("Options:");
  printGlobalOpts(maxOffset);
  Opt{ "--version", "-V" }
      .setDesc("Print version info and exit")
      .print(maxOffset);
  std::cout << '\n';

  printHeader("Commands:");
  for (const auto& [name, cmd] : getCmds()) {
    if (cmd.isShort) {
      continue;
    }
    // TODO: currently, we assume options are longer than commands.
    printCommand(name, cmd.cmd.getDesc(), cmd.cmd.hasShort(), maxOffset);
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
      return versionMain({});
    }

    // Subcommands
    else if (getCmds().contains(arg)) {
      try {
        // i points to the subcommand name that we don't need anymore.  Since
        // i starts from 1 from the start pointer of argv, we want to start
        // with i + 2.  As we know args.size() + 1 == argc and args.size() >=
        // i + 1, we can write the range as [i + 2, argc), which is not
        // out-of-range access.
        const Vec<StringRef> cmdArgs(argv + i + 2, argv + argc);
        return getCmds().at(arg).main(cmdArgs);
      } catch (const std::exception& e) {
        Logger::error(e.what());
        return EXIT_FAILURE;
      }
    }
    else {
      noSuchCommand(arg);
      return EXIT_FAILURE;
    }
  }

  helpMain({});
  return EXIT_SUCCESS;
}
