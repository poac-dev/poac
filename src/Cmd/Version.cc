#include "Version.hpp"

#include <iostream>

int versionMain(Vec<String>) noexcept {
  std::cout << "poac " << POAC_VERSION << '\n';
  return EXIT_SUCCESS;
}

void versionHelp() noexcept {
  std::cout << versionDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac version" << '\n';
}
