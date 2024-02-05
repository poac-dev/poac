#include "Test.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Parallelism.hpp"
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
        .addOpt(Opt{ "--jobs" }
                    .setShort("-j")
                    .setDesc("Set the number of jobs to run in parallel")
                    .setDefault(NUM_DEFAULT_THREADS))
        .setMainFn(testMain);

static int
testMain(const std::span<const StringRef> args) {
  // Parse args
  bool isDebug = true;
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Cli::handleGlobalOpts(itr, args.end(), "test")) {
      if (res.value() == Cli::CONTINUE) {
        continue;
      } else {
        return res.value();
      }
    } else if (*itr == "-d" || *itr == "--debug") {
      isDebug = true;
    } else if (*itr == "-r" || *itr == "--release") {
      logger::warn(
          "Tests in release mode could disable assert macros while speeding up "
          "the runtime."
      );
      isDebug = false;
    } else if (*itr == "-j" || *itr == "--jobs") {
      if (itr + 1 == args.end()) {
        logger::error("Missing argument for ", *itr);
        return EXIT_FAILURE;
      }
      setParallelism(std::stoul((++itr)->data()));
    } else {
      return TEST_CMD.noSuchArg(*itr);
    }
  }

  const auto start = std::chrono::steady_clock::now();

  const String outDir = emitMakefile(isDebug);
  const int exitCode = execCmd(getMakeCommand() + " -C " + outDir + " test");

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    logger::info(
        "Finished", modeString(isDebug), " test(s) in ", elapsed.count(), "s"
    );
  }
  return exitCode;
}
