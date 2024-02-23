#include "Build.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "../Parallelism.hpp"
#include "Common.hpp"

#include <chrono>
#include <cstdlib>
#include <span>

static int buildMain(std::span<const StringRef> args);

const Subcmd BUILD_CMD =
    Subcmd{ "build" }
        .setShort("b")
        .setDesc("Compile a local package and all of its dependencies")
        .addOpt(OPT_DEBUG)
        .addOpt(OPT_RELEASE)
        .addOpt(Opt{ "--compdb" }.setDesc(
            "Generate compilation database instead of building"
        ))
        .addOpt(OPT_JOBS)
        .setMainFn(buildMain);

int
buildImpl(String& outDir, const bool isDebug) {
  const auto start = std::chrono::steady_clock::now();

  outDir = emitMakefile(isDebug);
  const String makeCommand = getMakeCommand() + " -C " + outDir;
  const int exitCode = execCmd(makeCommand);

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    logger::info(
        "Finished", modeString(isDebug), " target(s) in ", elapsed.count(), "s"
    );
  }
  return exitCode;
}

static int
buildMain(const std::span<const StringRef> args) {
  // Parse args
  bool isDebug = true;
  bool buildCompdb = false;
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Cli::handleGlobalOpts(itr, args.end(), "build")) {
      if (res.value() == Cli::CONTINUE) {
        continue;
      } else {
        return res.value();
      }
    } else if (*itr == "-d" || *itr == "--debug") {
      isDebug = true;
    } else if (*itr == "-r" || *itr == "--release") {
      isDebug = false;
    } else if (*itr == "--compdb") {
      buildCompdb = true;
    } else if (*itr == "-j" || *itr == "--jobs") {
      if (itr + 1 == args.end()) {
        return Subcmd::missingArgumentForOpt(*itr);
      }
      setParallelism(std::stoul((++itr)->data()));
    } else {
      return BUILD_CMD.noSuchArg(*itr);
    }
  }

  if (!buildCompdb) {
    String outDir;
    return buildImpl(outDir, isDebug);
  }

  // Build compilation database
  const String outDir = emitCompdb(isDebug);
  logger::info("Generated", outDir, "/compile_commands.json");
  return EXIT_SUCCESS;
}
