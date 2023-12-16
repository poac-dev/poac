#include "Run.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"

#include <cstdlib>
#include <iostream>

int run(Vec<String> args) {
  const String outDir = emitMakefile(args);
  const int exitCode = std::system(("make -C " + outDir).c_str());
  if (exitCode != EXIT_SUCCESS) {
    Logger::error("Build failed with exit code ", exitCode);
    return exitCode;
  }

  const String projectName = "poac"; // TODO: get from poac.toml
  return std::system((outDir + "/" + projectName).c_str());
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
  std::cout << "    -d, --debug\t\tBuild with debug information (default)"
            << '\n';
  std::cout << "    -r, --release\tBuild with optimizations" << '\n';
}
