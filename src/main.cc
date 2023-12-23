#include "./Algos.hpp"
#include "./Cmd/Build.hpp"
#include "./Cmd/Clean.hpp"
#include "./Cmd/Fmt.hpp"
#include "./Cmd/Global.hpp"
#include "./Cmd/Help.hpp"
#include "./Cmd/Init.hpp"
#include "./Cmd/Lint.hpp"
#include "./Cmd/New.hpp"
#include "./Cmd/Run.hpp"
#include "./Cmd/Test.hpp"
#include "./Cmd/Version.hpp"
#include "./Logger.hpp"
#include "./Rustify.hpp"
#include "./TermColor.hpp"

#include <algorithm>
#include <cstdlib>
#include <ctype.h>
#include <exception>
#include <iostream>
#include <span>

struct Cmd {
  Fn<int(std::span<const StringRef>)> main;
  Fn<void()> help;
  StringRef desc;
};

#define DEFINE_CMD(name)                 \
  {                                      \
    #name, {                             \
      name##Main, name##Help, name##Desc \
    }                                    \
  }

static inline const HashMap<StringRef, Cmd> CMDS = {
    DEFINE_CMD(help), DEFINE_CMD(build), DEFINE_CMD(test), DEFINE_CMD(run),
    DEFINE_CMD(new),  DEFINE_CMD(clean), DEFINE_CMD(init), DEFINE_CMD(version),
    DEFINE_CMD(fmt),  DEFINE_CMD(lint),
};

void noSuchCommand(StringRef arg) {
  Vec<StringRef> candidates(CMDS.size());
  usize i = 0;
  for (const auto& cmd : CMDS) {
    candidates[i++] = cmd.first;
  }

  String suggestion;
  if (const auto similar = findSimilarStr(arg, candidates)) {
    suggestion = "       Did you mean `" + String(similar.value()) + "`?\n\n";
  }
  Logger::error(
      "no such command: `", arg, "`", "\n\n", suggestion,
      "       Run `poac help` for a list of commands"
  );
}

int helpMain(std::span<const StringRef> args) noexcept {
  // Parse args
  for (StringRef arg : args) {
    HANDLE_GLOBAL_OPTS({{"help"}})

    else if (CMDS.contains(arg)) {
      CMDS.at(arg).help();
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
  printUsage("", "[OPTIONS] [COMMAND]");
  std::cout << '\n';

  printHeader("Options:");
  printGlobalOpts();
  printOption("--version", "-V", "Print version info and exit");
  std::cout << '\n';

  printHeader("Commands:");
  for (const auto& [name, cmd] : CMDS) {
    printCommand(name, cmd.desc);
  }
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
  // Handle POAC_TERM_COLOR
  if (const char* color_p = std::getenv("POAC_TERM_COLOR")) {
    String color(color_p);
    std::transform(color.begin(), color.end(), color.begin(), ::tolower);

    if (color == "always") {
      setColorMode(ColorMode::always);
    } else if (color == "auto") {
      setColorMode(ColorMode::automatic);
    } else if (color == "never") {
      setColorMode(ColorMode::never);
    } else {
      Logger::error("invalid value for POAC_TERM_COLOR: ", color_p);
      return EXIT_FAILURE;
    }
  }

  // Parse arguments (options should appear before the subcommand, as the help
  // message shows intuitively)
  for (int i = 1; i < argc; ++i) {
    StringRef arg = argv[i];

    // Global options (which are not command-specific)
    HANDLE_GLOBAL_OPTS({})

    // Local options
    else if (arg == "-V" || arg == "--version") {
      return versionMain({});
    }

    // Subcommands
    else if (CMDS.contains(arg)) {
      try {
        const Vec<StringRef> cmd_args(argv + i + 1, argv + argc);
        return CMDS.at(arg).main(cmd_args);
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
