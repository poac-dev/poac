#include "Init.hpp"

#include "../Logger.hpp"
#include "Global.hpp"
#include "New.hpp"

#include <fstream>
#include <iostream>

int initMain(Vec<String> args) {
  // Parse args
  bool isBin = true;
  for (StringRef arg : args) {
    HANDLE_GLOBAL_OPTS({"init"})

    else if (arg == "-b" || arg == "--bin") {
      isBin = true;
    }
    else if (arg == "-l" || arg == "--lib") {
      isBin = false;
    }
    else {
      Logger::error("invalid argument: ", arg);
      return EXIT_FAILURE;
    }
  }

  if (fs::exists("poac.toml")) {
    Logger::error("cannot initialize an existing poac package");
    return EXIT_FAILURE;
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
  std::cout << initDesc << '\n';
  std::cout << '\n';
  printUsage("init", "[OPTIONS]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption("--bin", "-b", "Use a binary (application) template [default]");
  printOption("--lib", "-l", "Use a library template");
}
