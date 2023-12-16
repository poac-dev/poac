#include "Cmd/Build.hpp"
#include "Rustify.hpp"

#include <iostream>

#define POAC_VERSION "0.6.0"

void help() {
  std::cout << "poac " << POAC_VERSION << '\n';
  std::cout << "A package manager and build system for C++" << '\n';
  std::cout << '\n';
  std::cout << "USAGE:" << '\n';
  std::cout << "    poac <SUBCOMMAND>" << '\n';
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
    std::cerr << "[ERROR] no subcommand provided" << '\n';
    help();
    return 1;
  }

  HashMap<StringRef, Fn<void()>> cmds;
  cmds["help"] = help;
  cmds["build"] = build;

  StringRef subcommand = argv[1];
  if (cmds.count(subcommand) == 0) {
    std::cerr << "[ERROR] unknown subcommand '" << subcommand << "'" << '\n';
    help();
    return 1;
  }

  cmds[subcommand]();
  return 0;
}
