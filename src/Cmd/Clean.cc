#include "Clean.hpp"

#include "../Logger.hpp"

#include <iostream>

int cleanMain(Vec<String> args) {
  Path outDir = "poac-out";

  if (!args.empty()) {
    if (!(args[0] == "-p" || args[0] == "--profile")) {
      Logger::error("Invalid option: ", args[0]);
      return EXIT_FAILURE;
    }

    if (args.size() == 1) {
      Logger::error("Missing argument for ", args[0]);
      return EXIT_FAILURE;
    }

    if (!(args[1] == "debug" || args[1] == "release")) {
      Logger::error("Invalid argument for ", args[0], ": ", args[1]);
      return EXIT_FAILURE;
    }

    outDir /= args[1];
  }

  if (fs::exists(outDir)) {
    Logger::status("Removing", fs::canonical(outDir).string());
    fs::remove_all(outDir);
  }
  return EXIT_SUCCESS;
}

void cleanHelp() {
  std::cout << cleanDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac clean [OPTIONS]" << '\n';
  std::cout << '\n';
  std::cout << "Options:" << '\n';
  std::cout
      << "    -p, --profile <PROFILE>\tRemove built artifacts in <PROFILE> mode"
      << '\n';
}
