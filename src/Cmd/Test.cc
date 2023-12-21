#include "Test.hpp"

#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "Global.hpp"

#include <chrono>
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

  const auto start = std::chrono::steady_clock::now();

  const String outDir = emitMakefile(isDebug);
  const int exitCode =
      std::system((getMakeCommand() + " -C " + outDir + " test").c_str());

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    Logger::status(
        "Finished", modeString(isDebug), " test(s) in ", elapsed.count(), "s"
    );
  }
  return exitCode;
}

void testHelp() noexcept {
  std::cout << testDesc << '\n';
  std::cout << '\n';
  printUsage("test", "[OPTIONS]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption("--debug", "-d", "Test with debug information [default]");
  printOption("--release", "-r", "Test with optimizations");
}
