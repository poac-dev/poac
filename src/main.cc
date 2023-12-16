#include "Cmd/Build.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"

#include <iostream>

#define POAC_VERSION "0.6.0"

void help() {
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
  std::cout << "    build\tCompile a local package and all of its dependencies"
            << '\n';
  std::cout
      << "    help\tPrints this message or the help of the given subcommand(s)"
      << '\n';
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    Logger::error("no subcommand provided");
    help();
    return 1;
  }

  // Parse global options
  for (int i = 1; i < argc; ++i) {
    String arg = argv[i];
    if (arg == "-v" || arg == "--version") {
      std::cout << "poac " << POAC_VERSION << '\n';
      return 0;
    }
    if (arg == "--verbose") {
      Logger::setLevel(LogLevel::DEBUG);
    }
    if (arg == "-q" || arg == "--quiet") {
      Logger::setLevel(LogLevel::ERROR);
    }
  }

  HashMap<StringRef, Fn<void()>> cmds;
  cmds["help"] = help;
  cmds["build"] = build;

  StringRef subcommand = argv[1];
  if (cmds.count(subcommand) == 0) {
    Logger::error("unknown subcommand '", subcommand, "'");
    help();
    return 1;
  }

  cmds[subcommand]();
  return 0;
}
