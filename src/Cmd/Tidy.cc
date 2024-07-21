#include "Tidy.hpp"

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

static int tidyMain(std::span<const std::string_view> args);

const Subcmd TIDY_CMD =
    Subcmd{ "tidy" }
        .setDesc("Run clang-tidy")
        .addOpt(Opt{ "--fix" }.setDesc("Automatically apply lint suggestions"))
        .addOpt(OPT_JOBS)
        .setMainFn(tidyMain);

static int
tidyImpl(const std::string_view makeCmd) {
  const auto start = std::chrono::steady_clock::now();

  const int exitCode = execCmd(makeCmd);

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    logger::info("Finished", "clang-tidy in ", elapsed.count(), "s");
  }
  return exitCode;
}

static int
tidyMain(const std::span<const std::string_view> args) {
  // Parse args
  bool fix = false;
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Cli::handleGlobalOpts(itr, args.end(), "tidy")) {
      if (res.value() == Cli::CONTINUE) {
        continue;
      } else {
        return res.value();
      }
    } else if (*itr == "--fix") {
      fix = true;
    } else if (*itr == "-j" || *itr == "--jobs") {
      if (itr + 1 == args.end()) {
        return Subcmd::missingArgumentForOpt(*itr);
      }
      setParallelism(std::stoul((++itr)->data()));
    } else {
      return TIDY_CMD.noSuchArg(*itr);
    }
  }

  if (!commandExists("clang-tidy")) {
    logger::error("clang-tidy not found");
    return EXIT_FAILURE;
  }

  if (fix && isParallel()) {
    logger::warn("`--fix` implies `--jobs 1` to avoid race conditions");
    setParallelism(1);
  }

  const fs::path outDir = emitMakefile(true /* isDebug */);

  std::string tidyFlags = " POAC_TIDY_FLAGS='";
  if (!isVerbose()) {
    tidyFlags += "-quiet";
  }
  if (fs::exists(".clang-tidy")) {
    // clang-tidy will run within the poac-out/debug directory.
    tidyFlags += " --config-file=../../.clang-tidy";
  }
  if (fix) {
    tidyFlags += " -fix";
  }
  tidyFlags += '\'';

  std::string makeCmd = getMakeCommand();
  makeCmd += " -C ";
  makeCmd += outDir.string();
  makeCmd += tidyFlags;
  makeCmd += " tidy";

  logger::info("Running", "clang-tidy");
  return tidyImpl(makeCmd);
}
