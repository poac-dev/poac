#include "Test.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "Global.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <span>

int
testMain(const std::span<const StringRef> args) {
  // Parse args
  bool isDebug = true;
  bool isParallel = true;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "test" } })

    else if (arg == "-d" || arg == "--debug") {
      isDebug = true;
    }
    else if (arg == "-r" || arg == "--release") {
      Logger::warn(
          "Tests in release mode could disable assert macros while speeding up "
          "the runtime."
      );
      isDebug = false;
    }
    else if (arg == "--no-parallel") {
      isParallel = false;
    }
    else {
      Logger::error("invalid argument: ", arg);
      return EXIT_FAILURE;
    }
  }

  const auto start = std::chrono::steady_clock::now();

  const String outDir = emitMakefile(isDebug);
  const int exitCode =
      runCmd(getMakeCommand(isParallel) + " -C " + outDir + " test");

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    Logger::info(
        "Finished", modeString(isDebug), " test(s) in ", elapsed.count(), "s"
    );
  }
  return exitCode;
}

void
testHelp() noexcept {
  std::cerr << testDesc << '\n';
  std::cerr << '\n';
  printUsage("test", "[OPTIONS]");
  std::cerr << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption("--debug", "-d", "Test with debug information [default]");
  printOption("--release", "-r", "Test with optimizations");
  printOption("--no-parallel", "", "Disable parallel builds & tests");
}
