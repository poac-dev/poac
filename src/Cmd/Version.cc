#include "Version.hpp"

#include "../Logger.hpp"
#include "Global.hpp"

#include <iostream>

int versionMain(Vec<String> args) noexcept {
  // Parse args
  for (StringRef arg : args) {
    HANDLE_GLOBAL_OPTS({"version"})

    else {
      Logger::error("invalid argument: ", arg);
      return EXIT_FAILURE;
    }
  }

  std::cout << "poac " << POAC_VERSION << '\n';
  return EXIT_SUCCESS;
}

void versionHelp() noexcept {
  std::cout << versionDesc << '\n';
  std::cout << '\n';
  printUsage("poac version");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
}
