#include "Build.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "Global.hpp"

#include <chrono>
#include <iostream>

int buildImpl(const bool isDebug, String& outDir) {
  const auto start = std::chrono::steady_clock::now();

  outDir = emitMakefile(isDebug);
  const int exitCode =
      std::system((getMakeCommand() + " -C " + outDir).c_str());

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    Logger::status(
        "Finished", modeString(isDebug), " target(s) in ", elapsed.count(), "s"
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
  printUsage("build", "[OPTIONS]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption("--debug", "-d", "Build with debug information [default]");
  printOption("--release", "-r", "Build with optimizations");
}
