#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Rustify.hpp"
#include "Cmd.hpp"

#include <cstdlib>
#include <span>
#include <string>

static int tidyMain(std::span<const StringRef> args);

const Subcmd tidyCmd =
    Subcmd{ "tidy" }
        .setDesc("Run clang-tidy")
        .addOpt(Opt{ "--fix" }.setDesc("Automatically apply lint suggestions"))
        .setMainFn(tidyMain);

static int
tidyMain(const std::span<const StringRef> args) {
  // Parse args
  bool fix = false;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "tidy" } })

    else if (arg == "--fix") {
      fix = true;
    }
    else {
      return tidyCmd.noSuchArg(arg);
    }
  }

  if (!commandExists("clang-tidy")) {
    Logger::error("clang-tidy not found");
    return EXIT_FAILURE;
  }

  const Path outDir = emitMakefile(true /* isDebug */);

  String tidyFlags = " POAC_TIDY_FLAGS='";
  if (fs::exists(".clang-tidy")) {
    // clang-tidy will run within the poac-out/debug directory.
    tidyFlags += "--config-file=../../.clang-tidy";
  }
  if (fix) {
    tidyFlags += " -fix";
  }
  tidyFlags += '\'';

  // `poac tidy` invokes only one clang-tidy command, so parallelism over
  // Make does not make sense.
  String makeCmd = getMakeCommand(false /* isParallel */);
  makeCmd += " -C ";
  makeCmd += outDir.string();
  makeCmd += tidyFlags;
  makeCmd += " tidy";

  Logger::info("Running", "clang-tidy");
  return execCmd(makeCmd);
}
