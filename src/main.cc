#include "Cmd/Build.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"

#include <iostream>
#include <stdexcept>

#define POAC_VERSION "0.6.0"

int help(Vec<String> args) {
  if (args.empty()) {
    std::cout << "poac " << POAC_VERSION << '\n';
    std::cout << "A package manager and build system for C++" << '\n';
    std::cout << '\n';
    std::cout << "USAGE:" << '\n';
    std::cout << "    poac <SUBCOMMAND> [OPTIONS]" << '\n';
    std::cout << '\n';
    std::cout << "OPTIONS:" << '\n';
    std::cout << "    -v, --version\tPrint version info and exit" << '\n';
    std::cout << "    --verbose\t\tUse verbose output" << '\n';
    std::cout << "    -q, --quiet\t\tNo output printed to stdout" << '\n';
    std::cout << '\n';
    std::cout << "SUBCOMMANDS:" << '\n';
    std::cout
        << "    build\tCompile a local package and all of its dependencies"
        << '\n';
    std::cout
        << "    help\tPrints this message or the help of the given subcommand(s)"
        << '\n';
    return EXIT_SUCCESS;
  }

  HashMap<StringRef, Fn<void()>> helps;
  helps["build"] = buildHelp;

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
  for (int i = 1; i < argc; ++i) {
    String arg = argv[i];
    if (arg == "-v" || arg == "--version") {
      std::cout << "poac " << POAC_VERSION << '\n';
      return EXIT_SUCCESS;
    }
    if (arg == "--verbose") {
      Logger::setLevel(LogLevel::debug);
    } else if (arg == "-q" || arg == "--quiet") {
      Logger::setLevel(LogLevel::error);
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
  cmds["help"] = help;
  cmds["build"] = build;

  StringRef subcommand = args[0];
  if (cmds.count(subcommand) == 0) {
    Logger::error(
        "no such command: `", subcommand, "`", "\n\n",
        "       run `poac help` for a list of commands"
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
