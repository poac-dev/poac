#include "Build.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "Global.hpp"

#include <chrono>
#include <cstdlib>
#include <span>

static constinit const auto buildCli =
    Subcommand<4>("build")
        .setDesc(buildDesc)
        .setUsage("[OPTIONS]")
        .addOpt(Opt{ "--debug", "-d" }.setDesc(
            "Build with debug information [default]"
        ))
        .addOpt(Opt{ "--release", "-r" }.setDesc("Build with optimizations"))
        .addOpt(Opt{ "--compdb" }.setDesc(
            "Generate compilation database instead of building"
        ))
        .addOpt(Opt{ "--no-parallel" }.setDesc("Disable parallel builds"));

void
buildHelp() noexcept {
  buildCli.printHelp();
}

int
buildImpl(String& outDir, const bool isDebug, const bool isParallel) {
  const auto start = std::chrono::steady_clock::now();

  outDir = emitMakefile(isDebug);
  const String makeCommand = getMakeCommand(isParallel) + " -C " + outDir;
  const int exitCode = runCmd(makeCommand);

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    Logger::info(
        "Finished", modeString(isDebug), " target(s) in ", elapsed.count(), "s"
    );
  }
  return exitCode;
}

int
buildMain(const std::span<const StringRef> args) {
  // Parse args
  bool isDebug = true;
  bool buildCompdb = false;
  bool isParallel = true;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "build" } }) // workaround for std::span until C++26

    else if (arg == "-d" || arg == "--debug") {
      isDebug = true;
    }
    else if (arg == "-r" || arg == "--release") {
      isDebug = false;
    }
    else if (arg == "--compdb") {
      buildCompdb = true;
    }
    else if (arg == "--no-parallel") {
      isParallel = false;
    }
    else {
      return buildCli.noSuchArg(arg);
    }
  }

  if (!buildCompdb) {
    String outDir;
    return buildImpl(outDir, isDebug, isParallel);
  }

  // Build compilation database
  const String outDir = emitCompdb(isDebug);
  Logger::info("Generated", outDir, "/compile_commands.json");
  return EXIT_SUCCESS;
}
