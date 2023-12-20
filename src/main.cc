#include "Algos.hpp"
#include "Cmd/Build.hpp"
#include "Cmd/Clean.hpp"
#include "Cmd/Help.hpp"
#include "Cmd/Init.hpp"
#include "Cmd/New.hpp"
#include "Cmd/Run.hpp"
#include "Cmd/Test.hpp"
#include "Cmd/Version.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"
#include "TermColor.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>

struct Cmd {
  Fn<int(Vec<String>)> main;
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
};

static inline const HashMap<StringRef, StringRef> LONG_TO_SHORT = {
    {"build", "b"},      {"run", "r"},      {"test", "t"},
    {"--verbose", "-v"}, {"--quiet", "-q"}, {"--help", "-h"},
};

int helpMain(Vec<String> args) noexcept {
  if (args.empty()) {
    std::cout << "A package manager and build system for C++" << '\n';
    std::cout << '\n';
    std::cout << bold(green("Usage:")) << " poac [OPTIONS] [COMMAND]" << '\n';
    std::cout << '\n';
    std::cout << bold(green("Options:")) << '\n';
    std::cout << "  " << std::left << std::setw(15) << "-v, --version"
              << "Print version info and exit" << '\n';
    std::cout << "  " << std::left << std::setw(15) << "--verbose"
              << "Use verbose output" << '\n';
    std::cout << "  " << std::left << std::setw(15) << "-q, --quiet"
              << "Do not print poac log messages" << '\n';
    std::cout << '\n';
    std::cout << bold(green("Commands:")) << '\n';
    for (const auto& [name, cmd] : CMDS) {
      std::cout << "  " << std::left << std::setw(10) << name << cmd.desc
                << '\n';
    }
    return EXIT_SUCCESS;
  }

  StringRef subcommand = args[0];
  if (!CMDS.contains(subcommand)) {
    Vec<StringRef> candidates(CMDS.size());
    usize i = 0;
    for (const auto& cmd : CMDS) {
      candidates[i++] = cmd.first;
    }

    String suggestion;
    if (const auto similar = findSimilarStr(subcommand, candidates)) {
      suggestion = "       Did you mean `" + String(similar.value()) + "`?\n\n";
    }
    Logger::error(
        "no such command: `", subcommand, "`", "\n\n", suggestion,
        "       Run `poac help` for a list of commands"
    );
    return EXIT_FAILURE;
  }

  CMDS.at(subcommand).help();
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
  // Parse arguments (options should appear before the subcommand)
  for (int i = 1; i < argc; ++i) {
    StringRef arg = argv[i];
    // Global options
    if (arg == "--verbose") {
      Logger::setLevel(LogLevel::debug);
    } else if (arg == "-q" || arg == "--quiet") {
      Logger::setLevel(LogLevel::off);
    }

    // Local options
    else if (arg == "-v" || arg == "--version") {
      return versionMain({});
    }

    // Subcommands
    else if (CMDS.contains(arg)) {
      try {
        const Vec<String> cmd_args(argv + i + 1, argv + argc);
        return CMDS.at(arg).main(cmd_args);
      } catch (const std::exception& e) {
        Logger::error(e.what());
        return EXIT_FAILURE;
      }
    } else {
      Vec<StringRef> candidates(CMDS.size());
      usize i = 0;
      for (const auto& cmd : CMDS) {
        candidates[i++] = cmd.first;
      }

      String suggestion;
      if (const auto similar = findSimilarStr(arg, candidates)) {
        suggestion =
            "       Did you mean `" + String(similar.value()) + "`?\n\n";
      }
      Logger::error(
          "no such command: `", arg, "`", "\n\n", suggestion,
          "       Run `poac help` for a list of commands"
      );
      return EXIT_FAILURE;
    }
  }

  helpMain({});
  return EXIT_SUCCESS;
}
