#include "Build.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"

#include <ctime>
#include <iostream>

int buildCmd(Vec<String> args) {
  const bool isDebug = isDebugMode(args.empty() ? "" : args[0]);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  const String outDir = emitMakefile(isDebug);
  const int exitCode = std::system(("make -C " + outDir).c_str());

  clock_gettime(CLOCK_MONOTONIC, &end);
  const double elapsed =
      end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9;

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
