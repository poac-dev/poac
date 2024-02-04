#include "Run.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Parallel.hpp"
#include "../Rustify.hpp"
#include "Build.hpp"

#include <cstdlib>
#include <span>

static int runMain(std::span<const StringRef> args);

const Subcmd RUN_CMD =
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
  auto itr = args.begin();
  for (; itr != args.end(); ++itr) {
    if (const auto res = Cli::handleGlobalOpts(itr, args.end(), "run")) {
      if (res.value() == Cli::CONTINUE) {
        continue;
      } else {
        return res.value();
      }
    } else if (*itr == "-d" || *itr == "--debug") {
      isDebug = true;
    } else if (*itr == "-r" || *itr == "--release") {
      isDebug = false;
    } else if (*itr == "--no-parallel") {
      setParallel(false);
    } else {
      break;
    }
  }

  String runArgs;
  for (; itr != args.end(); ++itr) {
    runArgs += ' ' + String(*itr);
  }

  String outDir;
  if (buildImpl(outDir, isDebug) != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  const String& projectName = getPackageName();
  const String command = outDir + "/" + projectName + runArgs;
  return execCmd(command);
}
