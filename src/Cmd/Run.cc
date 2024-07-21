#include "Run.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Parallelism.hpp"
#include "../Rustify.hpp"
#include "Build.hpp"
#include "Common.hpp"

#include <cstdlib>
#include <span>
#include <string>
#include <string_view>

static int runMain(std::span<const std::string_view> args);

const Subcmd RUN_CMD =
    Subcmd{ "run" }
        .setShort("r")
        .setDesc("Build and execute src/main.cc")
        .addOpt(OPT_DEBUG)
        .addOpt(OPT_RELEASE)
        .addOpt(OPT_JOBS)
        .setArg(Arg{ "args" }
                    .setDesc("Arguments passed to the program")
                    .setVariadic(true)
                    .setRequired(false))
        .setMainFn(runMain);

static int
runMain(const std::span<const std::string_view> args) {
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
    } else if (*itr == "-j" || *itr == "--jobs") {
      if (itr + 1 == args.end()) {
        return Subcmd::missingArgumentForOpt(*itr);
      }
      setParallelism(std::stoul((++itr)->data()));
    } else {
      break;
    }
  }

  std::string runArgs;
  for (; itr != args.end(); ++itr) {
    runArgs += ' ' + std::string(*itr);
  }

  std::string outDir;
  if (buildImpl(outDir, isDebug) != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  const std::string& projectName = getPackageName();
  const std::string command = outDir + "/" + projectName + runArgs;
  return execCmd(command);
}
