#include "Test.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Parallelism.hpp"
#include "../Rustify.hpp"
#include "Common.hpp"

#include <charconv>
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
      ++itr;

      std::uint64_t numThreads{};
      auto [ptr, ec] =
          std::from_chars(itr->data(), itr->data() + itr->size(), numThreads);
      if (ec == std::errc()) {
        setParallelism(numThreads);
      } else {
        logger::error("invalid number of threads: ", *itr);
        return EXIT_FAILURE;
      }
    } else {
      return TEST_CMD.noSuchArg(*itr);
    }
  }

  const auto start = std::chrono::steady_clock::now();

  const BuildConfig config = emitMakefile(isDebug, /*includeDevDeps=*/true);
  const std::string outDir = config.getOutDir();
  const std::string& packageName = getPackageName();
  const Command baseMakeCmd = getMakeCommand().addArg("-C").addArg(outDir);

  // Find not up-to-date test targets and just emit a message that we need
  // compilation.
  for (const auto& [target, sourcePath] : config.testTargetToSourcePaths) {
    static_cast<void>(sourcePath);

    Command checkUpToDateCmd = baseMakeCmd;
    checkUpToDateCmd.addArg("--question").addArg(target);

    const int exitCode = execCmd(checkUpToDateCmd);
    if (exitCode != EXIT_SUCCESS) {
      // This test target is not up-to-date.
      logger::info("Compiling", packageName);
      break;
    }
  }

  // Compile and run tests.
  int exitCode{};
  for (const auto& [target, sourcePath] : config.testTargetToSourcePaths) {
    logger::info("Running", "unittests ", sourcePath);

    Command testCmd = baseMakeCmd;
    testCmd.addArg(target);

    const int curExitCode = execCmd(testCmd);
    if (curExitCode != EXIT_SUCCESS) {
      exitCode = curExitCode;
    }
  }

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    logger::info(
        "Finished", modeToString(isDebug), " test(s) in ", elapsed.count(), "s"
    );
  }
  return exitCode;
}
