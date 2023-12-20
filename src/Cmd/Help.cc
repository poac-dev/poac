#include "Help.hpp"

#include <iostream>

void helpHelp() noexcept {
  std::cout << helpDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac help [OPTIONS] [COMMAND]" << '\n';
  std::cout << '\n';
  std::cout << "Arguments:" << '\n';
  std::cout << "  [COMMAND]" << '\n';
}
