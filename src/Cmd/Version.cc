#include "Version.hpp"

#include "../Logger.hpp"
#include "Global.hpp"

#include <cstdlib>
#include <iostream>
#include <span>

#ifndef POAC_VERSION
#  error "POAC_VERSION is not defined"
#endif

int versionMain(std::span<const StringRef> args) noexcept {
  // Parse args
  for (usize i = 0; i < args.size(); ++i) {
    StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({{"version"}})

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
  printUsage("version", "[OPTIONS]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
}
