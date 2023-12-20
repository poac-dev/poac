#include "Test.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "Global.hpp"

#include <ctime>
#include <iostream>

int testMain(Vec<String> args) {
  // Parse args
  bool isDebug = true;
  for (StringRef arg : args) {
    HANDLE_GLOBAL_OPTS({"test"})

    else if (arg == "-d" || arg == "--debug") {
      isDebug = true;
    }
    else if (arg == "-r" || arg == "--release") {
      Logger::warn("The `--release` flag can disable assert macros.");
      isDebug = false;
    }
    else {
      Logger::error("invalid argument: ", arg);
      return EXIT_FAILURE;
    }
  }

  timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  const String outDir = emitMakefile(isDebug);
  const int exitCode =
      std::system((getMakeCommand() + " -C " + outDir + " test").c_str());

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

void testHelp() noexcept {
  std::cout << testDesc << '\n';
  std::cout << '\n';
  printUsage("poac test [OPTIONS]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption("-d, --debug", "Test with debug information [default]");
  printOption("-r, --release", "Test with optimizations");
}
