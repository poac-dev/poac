#include "Init.hpp"

#include "../Logger.hpp"
#include "New.hpp"

#include <fstream>
#include <iostream>

int initMain(Vec<String> args) {
  if (fs::exists("poac.toml")) {
    Logger::error("cannot initialize an existing poac package");
    return EXIT_FAILURE;
  }

  // Parse options
  bool isBin = true;
  if (args.empty()) {
    isBin = true;
  } else {
    if (args[0] == "-b" || args[0] == "--bin") {
      isBin = true;
    } else if (args[0] == "-l" || args[0] == "--lib") {
      isBin = false;
    } else {
      Logger::error("invalid option: ", args[0]);
      return EXIT_FAILURE;
    }
  }

  const String packageName = fs::current_path().stem().string();
  if (!verifyPackageName(packageName)) {
    return EXIT_FAILURE;
  }

  std::ofstream ofs("poac.toml");
  ofs << getPoacToml(packageName);

  Logger::status(
      "Created", isBin ? "binary (application) `" : "library `", packageName,
      "` package"
  );
  return EXIT_SUCCESS;
}

void initHelp() noexcept {
  std::cout << initDesc << std::endl;
  std::cout << std::endl;
  std::cout << "USAGE:" << std::endl;
  std::cout << "    poac init [OPTIONS]" << std::endl;
  std::cout << std::endl;
  std::cout << "OPTIONS:" << std::endl;
  std::cout << "    -b, --bin\tUse a binary (application) template [default]"
            << std::endl;
  std::cout << "    -l, --lib\tUse a library template" << std::endl;
}
