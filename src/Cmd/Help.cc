#include "Help.hpp"

#include "Global.hpp"

#include <iostream>

void helpHelp() noexcept {
  std::cout << helpDesc << '\n';
  std::cout << '\n';
  printUsage("poac help [OPTIONS] [COMMAND]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  std::cout << '\n';
  printHeader("Arguments:");
  std::cout << "  [COMMAND]" << '\n';
}
