#include "Tidy.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "Global.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int tidyMain(const std::span<const StringRef> args) {
  // Parse args
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "tidy" } })

    else {
      Logger::error("invalid argument: ", arg);
      return EXIT_FAILURE;
    }
  }

  if (!commandExists("clang-tidy")) {
    Logger::error("clang-tidy not found");
    return EXIT_FAILURE;
  }

  const Path outDir = emitMakefile(true /* isDebug */);

  // clang-tidy will run within the poac-out/debug directory.  So, we need
  // to symlink the .clang-tidy file to the poac-out/debug directory.
  if (fs::exists(".clang-tidy") && !fs::is_symlink(outDir / ".clang-tidy")) {
    fs::create_symlink("../../.clang-tidy", outDir / ".clang-tidy");
  }

  // `poac tidy` invokes only one clang-tidy command, so parallelism over
  // Make does not make sense.
  String makeCmd = getMakeCommand(false /* isParallel */);
  makeCmd += " -C ";
  makeCmd += outDir.string();
  makeCmd += " tidy";

  Logger::info("Running", "clang-tidy");
  return runCmd(makeCmd);
}

void tidyHelp() noexcept {
  std::cout << tidyDesc << '\n';
  std::cout << '\n';
  printUsage("tidy", "[OPTIONS]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
}
