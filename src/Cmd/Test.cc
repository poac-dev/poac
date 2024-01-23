#include "Test.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "Global.hpp"

#include <chrono>
#include <cstdlib>
#include <span>

static const auto TEST_CMD =
    Subcmd("test")
        .setDesc(testDesc)
        .addOpt(Opt{ "--debug", "-d" }.setDesc(
            "Test with debug information [default]"
        ))
        .addOpt(Opt{ "--release", "-r" }.setDesc("Test with optimizations"))
        .addOpt(Opt{ "--no-parallel" }.setDesc("Disable parallel builds & tests"
        ));

void
testHelp() noexcept {
  TEST_CMD.printHelp();
}

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
      return TEST_CMD.noSuchArg(arg);
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
