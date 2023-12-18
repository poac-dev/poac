#include "Algos.hpp"
#include "Cmd/Build.hpp"
#include "Cmd/New.hpp"
#include "Cmd/Run.hpp"
#include "Cmd/Test.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"

#include <iostream>
#include <stdexcept>

#define POAC_VERSION "0.6.0"

int helpCmd(Vec<String> args) {
  if (args.empty()) {
    std::cout << "poac " << POAC_VERSION << '\n';
    std::cout << "A package manager and build system for C++" << '\n';
    std::cout << '\n';
    std::cout << "Usage: poac [OPTIONS] [COMMAND]" << '\n';
    std::cout << '\n';
    std::cout << "Options:" << '\n';
    std::cout << "    -v, --version\tPrint version info and exit" << '\n';
    std::cout << "    --verbose\t\tUse verbose output" << '\n';
    std::cout << "    -q, --quiet\t\tNo output printed to stdout" << '\n';
    std::cout << '\n';
    std::cout << "Commands:" << '\n';
    std::cout
        << "    help\tPrints this message or the help of the given subcommand(s)"
        << '\n';
    std::cout << "    build\t" << buildDesc << '\n';
    std::cout << "    test\t" << testDesc << '\n';
    std::cout << "    run\t" << runDesc << '\n';
    std::cout << "    new\t" << newDesc << '\n';
    return EXIT_SUCCESS;
  }

  HashMap<StringRef, Fn<void()>> helps;
  helps["build"] = buildHelp;
  helps["test"] = testHelp;
  helps["run"] = runHelp;
  helps["new"] = newHelp;

  StringRef subcommand = args[0];
  if (helps.count(subcommand) == 0) {
    Logger::error(
        "no such subcommand: `", subcommand, "`", "\n\n",
        "       run `poac help` for a list of subcommands"
    );
    return EXIT_FAILURE;
  }

  helps[subcommand]();
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
  // Parse global options
  Vec<String> args;
  bool isVerbositySet = false;
  for (int i = 1; i < argc; ++i) {
    String arg = argv[i];
    if (arg == "-v" || arg == "--version") {
      std::cout << "poac " << POAC_VERSION << '\n';
      return EXIT_SUCCESS;
    }

    // This is a bit of a hack to allow the global options to be specified
    // in poac run, e.g., `poac run --verbose test --verbose`.  This will
    // remove the first --verbose and execute the run command as verbose,
    // then run the test command as verbose.
    if (!isVerbositySet) {
      if (arg == "--verbose") {
        Logger::setLevel(LogLevel::debug);
        isVerbositySet = true;
      } else if (arg == "-q" || arg == "--quiet") {
        Logger::setLevel(LogLevel::error);
        isVerbositySet = true;
      } else {
        args.push_back(arg);
      }
    } else {
      args.push_back(arg);
    }
  }

  if (args.empty()) {
    Logger::error(
        "no subcommand provided", "\n\n",
        "       run `poac help` for a list of commands"
    );
    return EXIT_FAILURE;
  }

  HashMap<StringRef, Fn<int(Vec<String>)>> cmds;
  cmds["help"] = helpCmd;
  cmds["build"] = buildCmd;
  cmds["test"] = testCmd;
  cmds["run"] = runCmd;
  cmds["new"] = newCmd;

  StringRef subcommand = args[0];
  if (cmds.count(subcommand) == 0) {
    Vec<StringRef> candidates(cmds.size());
    usize i = 0;
    for (const auto& cmd : cmds) {
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

  try {
    return cmds[subcommand](Vec<String>(args.begin() + 1, args.end()));
  } catch (const std::exception& e) {
    Logger::error(e.what());
    return EXIT_FAILURE;
  }
}
