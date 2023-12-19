#include "Run.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "Build.hpp"

#include <cstdlib>
#include <iostream>

int runMain(Vec<String> args) {
  bool isDebug = true;
  usize argsConsumed = 0;
  if (!args.empty()) {
    if (args[0] == "-d" || args[0] == "--debug") {
      ++argsConsumed;
    } else if (args[0] == "-r" || args[0] == "--release") {
      isDebug = false;
      ++argsConsumed;
    }
  }

  String outDir;
  if (buildImpl(isDebug, outDir) != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  String projectArgs;
  for (usize i = argsConsumed; i < args.size(); ++i) {
    projectArgs += " " + args[i];
  }

  const String projectName = "poac"; // TODO: get from poac.toml
  const String command = outDir + "/" + projectName + projectArgs;
  Logger::status("Running", command);
  return std::system(command.c_str());
}

void runHelp() noexcept {
  std::cout << runDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac run [OPTIONS] [args]..." << '\n';
  std::cout << '\n';
  std::cout << "Arguments:" << '\n';
  std::cout << "    [args]...\t\tArguments passed to the program" << '\n';
  std::cout << '\n';
  std::cout << "Options:" << '\n';
  std::cout << "    -d, --debug\t\tBuild with debug information [default]"
            << '\n';
  std::cout << "    -r, --release\tBuild with optimizations" << '\n';
}
