#include "Run.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "Build.hpp"
#include "Global.hpp"

#include <cstdlib>
#include <iostream>
#include <span>

int runMain(const std::span<const StringRef> args) {
  // Parse args
  bool isDebug = true;
  bool isParallel = true;
  String runArgs;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "run" } })

    else if (arg == "-d" || arg == "--debug") {
      isDebug = true;
    }
    else if (arg == "-r" || arg == "--release") {
      isDebug = false;
    }
    else if (arg == "--no-parallel") {
      isParallel = false;
    }
    else {
      runArgs += " " + String(arg);
    }
  }

  String outDir;
  if (buildImpl(outDir, isDebug, isParallel) != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  const String& projectName = getPackageName();
  const String command = outDir + "/" + projectName + runArgs;
  const int exitCode = runCmd(command);
  return exitCode;
}

void runHelp() noexcept {
  std::cout << runDesc << '\n';
  std::cout << '\n';
  printUsage("run", "[OPTIONS] [args]...");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption("--debug", "-d", "Build with debug information [default]");
  printOption("--release", "-r", "Build with optimizations");
  printOption("--no-parallel", "", "Disable parallel builds");
  std::cout << '\n';
  printHeader("Arguments:");
  std::cout << "  [args]...\tArguments passed to the program" << '\n';
}
