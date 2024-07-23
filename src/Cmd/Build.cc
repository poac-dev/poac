#include "Build.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Parallelism.hpp"
#include "Common.hpp"

#include <chrono>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <span>
#include <string>
#include <string_view>

static int buildMain(std::span<const std::string_view> args);

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
buildImpl(std::string& outDir, const bool isDebug) {
  const auto start = std::chrono::steady_clock::now();

  outDir = emitMakefile(isDebug, /*includeDevDeps=*/false);
  const std::string makeCommand = getMakeCommand() + " -C " + outDir;
  const int exitCode = execCmd(makeCommand);

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    const Profile& profile = isDebug ? getDevProfile() : getReleaseProfile();

    Vec<std::string> profiles;
    if (profile.opt_level.value() == 0) {
      profiles.push_back("unoptimized");
    } else {
      profiles.push_back("optimized");
    }
    if (profile.debug.value()) {
      profiles.push_back("debuginfo");
    }
    const std::string profileStr =
        fmt::format("[{}]", fmt::join(profiles, " + "));

    logger::info(
        "Finished", "`", modeToProfile(isDebug), "` profile ", profileStr,
        " target(s) in ", elapsed.count(), "s"
    );
  }
  return exitCode;
}

static int
buildMain(const std::span<const std::string_view> args) {
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
    } else if (itr->starts_with("-j")) {
      setParallelism(std::stoul(itr->substr(2).data()));
    } else if (itr->starts_with("--jobs=")) {
      setParallelism(std::stoul(itr->substr(7).data()));
    } else {
      return BUILD_CMD.noSuchArg(*itr);
    }
  }

  if (!buildCompdb) {
    std::string outDir;
    return buildImpl(outDir, isDebug);
  }

  // Build compilation database
  const std::string outDir = emitCompdb(isDebug, /*includeDevDeps=*/false);
  logger::info("Generated", outDir, "/compile_commands.json");
  return EXIT_SUCCESS;
}
