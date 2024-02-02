#include "Test.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Parallel.hpp"
#include "../Rustify.hpp"

#include <chrono>
#include <cstdlib>
#include <span>

static int testMain(std::span<const StringRef> args);

const Subcmd TEST_CMD =
    Subcmd{ "test" }
        .setShort("t")
        .setDesc("Run the tests of a local package")
        .addOpt(Opt{ "--debug" }.setShort("-d").setDesc(
            "Test with debug information [default]"
        ))
        .addOpt(
            Opt{ "--release" }.setShort("-r").setDesc("Test with optimizations")
        )
        .addOpt(Opt{ "--no-parallel" }.setDesc("Disable parallel builds & tests"
        ))
        .setMainFn(testMain);

static int
testMain(const std::span<const StringRef> args) {
  // Parse args
  bool isDebug = true;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "test" } })

    else if (arg == "-d" || arg == "--debug") {
      isDebug = true;
    }
    else if (arg == "-r" || arg == "--release") {
      log::warn(
          "Tests in release mode could disable assert macros while speeding up "
          "the runtime."
      );
      isDebug = false;
    }
    else if (arg == "--no-parallel") {
      setParallel(false);
    }
    else {
      return TEST_CMD.noSuchArg(arg);
    }
  }

  const auto start = std::chrono::steady_clock::now();

  const String outDir = emitMakefile(isDebug);
  const int exitCode = execCmd(getMakeCommand() + " -C " + outDir + " test");

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    log::info(
        "Finished", modeString(isDebug), " test(s) in ", elapsed.count(), "s"
    );
  }
  return exitCode;
}
