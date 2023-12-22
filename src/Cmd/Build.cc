#include "Build.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "Global.hpp"

#include <chrono>
#include <iostream>
#include <span>

int buildImpl(const bool isDebug, String& outDir) {
  const auto start = std::chrono::steady_clock::now();

  outDir = emitMakefile(isDebug);
  const int status = std::system((getMakeCommand() + " -C " + outDir).c_str());
  const int exitCode = status >> 8;

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    Logger::info(
        "Finished", modeString(isDebug), " target(s) in ", elapsed.count(), "s"
    );
  }
  return exitCode;
}

int buildMain(std::span<const StringRef> args) {
  // Parse args
  bool isDebug = true;
  bool buildCompdb = false;
  for (StringRef arg : args) {
    HANDLE_GLOBAL_OPTS({{"build"}}) // workaround for std::span until C++26

    else if (arg == "-d" || arg == "--debug") {
      isDebug = true;
    }
    else if (arg == "-r" || arg == "--release") {
      isDebug = false;
    }
    else if (arg == "--compdb") {
      buildCompdb = true;
    }
    else {
      Logger::error("Unknown argument: ", arg);
      return EXIT_FAILURE;
    }
  }

  if (!buildCompdb) {
    String outDir;
    return buildImpl(isDebug, outDir);
  }

  // Build compilation database
  const String outDir = emitCompdb(isDebug);
  Logger::info("Generated", outDir, "/compile_commands.json");
  return EXIT_SUCCESS;
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
  printOption("--compdb", "", "Emit compilation database to stdout");
}
