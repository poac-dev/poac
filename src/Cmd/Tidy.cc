#include "Tidy.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Parallel.hpp"
#include "../Rustify.hpp"

#include <chrono>
#include <cstdlib>
#include <span>
#include <string>

static int tidyMain(std::span<const StringRef> args);

const Subcmd TIDY_CMD =
    Subcmd{ "tidy" }
        .setDesc("Run clang-tidy")
        .addOpt(Opt{ "--fix" }.setDesc("Automatically apply lint suggestions"))
        .addOpt(Opt{ "--no-parallel" }.setDesc("Disable parallel builds"))
        .setMainFn(tidyMain);

static int
tidyImpl(const StringRef makeCmd) {
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
tidyMain(const std::span<const StringRef> args) {
  // Parse args
  bool fix = false;
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Command::handleGlobalOpts(itr, args.end(), "tidy")) {
      return res.value();
    } else if (*itr == "--fix") {
      fix = true;
    } else if (*itr == "--no-parallel") {
      setParallel(false);
    } else {
      return TIDY_CMD.noSuchArg(*itr);
    }
  }

  if (!commandExists("clang-tidy")) {
    logger::error("clang-tidy not found");
    return EXIT_FAILURE;
  }

  if (fix && isParallel()) {
    logger::warn("`--fix` automatically implies `--no-parallel`");
    setParallel(false);
  }

  const Path outDir = emitMakefile(true /* isDebug */);

  String tidyFlags = " POAC_TIDY_FLAGS='";
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

  String makeCmd = getMakeCommand();
  makeCmd += " -C ";
  makeCmd += outDir.string();
  makeCmd += tidyFlags;
  makeCmd += " tidy";

  logger::info("Running", "clang-tidy");
  return tidyImpl(makeCmd);
}
