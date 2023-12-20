#include "Build.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "Global.hpp"

#include <ctime>
#include <iostream>

int buildImpl(const bool isDebug, String& outDir) {
  timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  outDir = emitMakefile(isDebug);
  const int exitCode =
      std::system(("make -s --no-print-directory -C " + outDir).c_str());

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

int buildMain(Vec<String> args) {
  bool isDebug = true;
  // Parse args
  for (StringRef arg : args) {
    HANDLE_GLOBAL_OPTS({"build"})

    else if (arg == "-d" || arg == "--debug") {
      isDebug = true;
    }
    else if (arg == "-r" || arg == "--release") {
      isDebug = false;
    }
    else {
      Logger::error("Unknown argument: ", arg);
      return EXIT_FAILURE;
    }
  }

  String outDir;
  return buildImpl(isDebug, outDir);
}

void buildHelp() noexcept {
  std::cout << buildDesc << '\n';
  std::cout << '\n';
  printUsage("poac build [OPTIONS]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption("-d, --debug", "Build with debug information [default]");
  printOption("-r, --release", "Build with optimizations");
}
