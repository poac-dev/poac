#include "Tidy.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "Global.hpp"

#include <cstdlib>
#include <string>

static const auto TIDY_CLI = Subcmd("tidy").setDesc(tidyDesc).addOpt(
    Opt{ "--fix" }.setDesc("Automatically apply lint suggestions")
);

void
tidyHelp() noexcept {
  TIDY_CLI.printHelp();
}

int
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
      return TIDY_CLI.noSuchArg(arg);
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
  return runCmd(makeCmd);
}
