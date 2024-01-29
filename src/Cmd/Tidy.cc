#include "Tidy.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
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
tidyImpl(const String& makeCmd) {
  const auto start = std::chrono::steady_clock::now();

  const int exitCode = execCmd(makeCmd);

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    Logger::info("Finished", "clang-tidy in ", elapsed.count(), "s");
  }
  return exitCode;
}

static int
tidyMain(const std::span<const StringRef> args) {
  // Parse args
  bool fix = false;
  bool isParallel = true;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "tidy" } })

    else if (arg == "--fix") {
      fix = true;
    }
    else if (arg == "--no-parallel") {
      isParallel = false;
    }
    else {
      return TIDY_CMD.noSuchArg(arg);
    }
  }

  if (!commandExists("clang-tidy")) {
    Logger::error("clang-tidy not found");
    return EXIT_FAILURE;
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

  String makeCmd = getMakeCommand(isParallel);
  makeCmd += " -C ";
  makeCmd += outDir.string();
  makeCmd += tidyFlags;
  makeCmd += " tidy";

  Logger::info("Running", "clang-tidy");
  return tidyImpl(makeCmd);
}
