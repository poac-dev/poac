#include "Run.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"

#include <cstdlib>
#include <iostream>

int runCmd(Vec<String> args) {
  String profile;
  usize argsConsumed = 0;
  if (!args.empty()) {
    if (args[0] == "-d" || args[0] == "--debug" || args[0] == "-r"
        || args[0] == "--release") {
      profile = args[0];
      ++argsConsumed;
    }
  }

  const String outDir = emitMakefile(profile);
  const int exitCode = std::system(("make -C " + outDir).c_str());
  if (exitCode != EXIT_SUCCESS) {
    Logger::error("Build failed with exit code ", exitCode);
    return exitCode;
  }

  String projectArgs;
  for (usize i = argsConsumed; i < args.size(); ++i) {
    projectArgs += " " + args[i];
  }

  const String projectName = "poac"; // TODO: get from poac.toml
  return std::system((outDir + "/" + projectName + projectArgs).c_str());
}

void runHelp() {
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
