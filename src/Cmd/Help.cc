#include "Help.hpp"

#include "Global.hpp"

#include <iostream>

void
helpHelp() noexcept {
  std::cerr << helpDesc << '\n';
  std::cerr << '\n';
  printUsage("help", "[OPTIONS] [COMMAND]");
  std::cerr << '\n';
  printHeader("Options:");
  printGlobalOpts();
  std::cerr << '\n';
  printHeader("Arguments:");
  std::cerr << "  [COMMAND]" << '\n';
}
