#include "Build.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"

#include <chrono>
#include <iostream>

int buildCmd(Vec<String> args) {
  const bool isDebug = isDebugMode(args.empty() ? "" : args[0]);

  const auto start = std::chrono::system_clock::now();

  const String outDir = emitMakefile(isDebug);
  const int exitCode = std::system(("make -C " + outDir).c_str());

  const auto end = std::chrono::system_clock::now();
  const auto elapsed = std::chrono::duration<double>(end - start).count();

  if (exitCode == EXIT_SUCCESS) {
    Logger::status(
        "Finished", modeString(isDebug), " target(s) in ", elapsed, "s"
    );
  }
  return exitCode;
}

void buildHelp() {
  std::cout << buildDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac build [OPTIONS]" << '\n';
  std::cout << '\n';
  std::cout << "Options:" << '\n';
  std::cout << "    -d, --debug\t\tBuild with debug information [default]"
            << '\n';
  std::cout << "    -r, --release\tBuild with optimizations" << '\n';
}
