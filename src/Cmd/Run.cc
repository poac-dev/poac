#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "Cmd.hpp"

#include <cstdlib>
#include <span>

static int runMain(std::span<const StringRef> args);

const Subcmd runCmd =
    Subcmd{ "run" }
        .setShort("r")
        .setDesc("Build and execute src/main.cc")
        .addOpt(Opt{ "--debug" }.setShort("-d").setDesc(
            "Build with debug information [default]"
        ))
        .addOpt(Opt{ "--release" }.setShort("-r").setDesc(
            "Build with optimizations"
        ))
        .addOpt(Opt{ "--no-parallel" }.setDesc("Disable parallel builds"))
        .setArg(Arg{ "args" }
                    .setDesc("Arguments passed to the program")
                    .setVariadic(true)
                    .setRequired(false))
        .setMainFn(runMain);

static int
runMain(const std::span<const StringRef> args) {
  // Parse args
  bool isDebug = true;
  bool isParallel = true;
  usize i = 0;
  for (i = 0; i < args.size(); ++i) {
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
      break;
    }
  }

  String runArgs;
  for (; i < args.size(); ++i) {
    runArgs += ' ' + String(args[i]);
  }

  String outDir;
  if (buildImpl(outDir, isDebug, isParallel) != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  const String& projectName = getPackageName();
  const String command = outDir + "/" + projectName + runArgs;
  const int exitCode = execCmd(command);
  return exitCode;
}
