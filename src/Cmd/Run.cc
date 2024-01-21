#include "Run.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "Build.hpp"
#include "Global.hpp"

#include <cstdlib>
#include <span>

static constexpr auto RUN_CLI =
    Subcmd<3>("run")
        .setDesc(runDesc)
        .addOpt(Opt{ "--debug", "-d" }.setDesc(
            "Build with debug information [default]"
        ))
        .addOpt(Opt{ "--release", "-r" }.setDesc("Build with optimizations"))
        .addOpt(Opt{ "--no-parallel" }.setDesc("Disable parallel builds"))
        .setArg(Arg{ "[args]..." }.setDesc("Arguments passed to the program"));

void
runHelp() noexcept {
  RUN_CLI.printHelp();
}

int
runMain(const std::span<const StringRef> args) {
  // Parse args
  bool isDebug = true;
  bool isParallel = true;
  String runArgs;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "run" } })

    else if (arg == "-d" || arg == "--debug") {
      isDebug = true;
    }
    else if (arg == "-r" || arg == "--release") {
      isDebug = false;
    }
    else if (arg == "--no-parallel") {
      isParallel = false;
    }
    else {
      runArgs += " " + String(arg);
    }
  }

  String outDir;
  if (buildImpl(outDir, isDebug, isParallel) != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  const String& projectName = getPackageName();
  const String command = outDir + "/" + projectName + runArgs;
  const int exitCode = runCmd(command);
  return exitCode;
}
