#include "Test.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Parallelism.hpp"
#include "../Rustify.hpp"
#include "Common.hpp"

#include <chrono>
#include <cstdlib>
#include <span>
#include <string>
#include <string_view>

static int testMain(std::span<const std::string_view> args);

const Subcmd TEST_CMD = //
    Subcmd{ "test" }
        .setShort("t")
        .setDesc("Run the tests of a local package")
        .addOpt(OPT_DEBUG)
        .addOpt(OPT_RELEASE)
        .addOpt(OPT_JOBS)
        .setMainFn(testMain);

static int
testMain(const std::span<const std::string_view> args) {
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
        return Subcmd::missingArgumentForOpt(*itr);
      }
      setParallelism(std::stoul((++itr)->data()));
    } else {
      return TEST_CMD.noSuchArg(*itr);
    }
  }

  const auto start = std::chrono::steady_clock::now();

  const std::string outDir = emitMakefile(isDebug, /*includeDevDeps=*/true);
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
