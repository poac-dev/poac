#include "Build.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "../Parallel.hpp"

#include <chrono>
#include <cstdlib>
#include <span>

static int buildMain(std::span<const StringRef> args);

const Subcmd BUILD_CMD =
    Subcmd{ "build" }
        .setShort("b")
        .setDesc("Compile a local package and all of its dependencies")
        .addOpt(Opt{ "--debug" }.setShort("-d").setDesc(
            "Build with debug information [default]"
        ))
        .addOpt(Opt{ "--release" }.setShort("-r").setDesc(
            "Build with optimizations"
        ))
        .addOpt(Opt{ "--compdb" }.setDesc(
            "Generate compilation database instead of building"
        ))
        .addOpt(Opt{ "--no-parallel" }.setDesc("Disable parallel builds"))
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
    } else if (*itr == "--no-parallel") {
      setParallel(false);
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
