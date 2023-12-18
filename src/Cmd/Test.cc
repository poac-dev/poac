#include "Test.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"

#include <ctime>
#include <iostream>

int testCmd(Vec<String> args) {
  const bool isDebug = isDebugMode(args.empty() ? "" : args[0]);
  if (!isDebug) {
    Logger::warn("The `--release` flag can disable assert macros.");
  }

  timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  const String outDir = emitMakefile(isDebug);
  const int exitCode = std::system(("make -C " + outDir + " test").c_str());

  clock_gettime(CLOCK_MONOTONIC, &end);
  const double elapsed =
      end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9;

  if (exitCode == EXIT_SUCCESS) {
    Logger::status(
        "Finished", modeString(isDebug), " test(s) in ", elapsed, "s"
    );
  }
  return exitCode;
}

void testHelp() {
  std::cout << testDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac test [OPTIONS]" << '\n';
  std::cout << '\n';
  std::cout << "Options:" << '\n';
  std::cout << "    -d, --debug\t\tTest with debug information [default]"
            << '\n';
  std::cout << "    -r, --release\tTest with optimizations" << '\n';
}
