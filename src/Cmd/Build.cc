#include "Build.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Parallelism.hpp"
#include "Common.hpp"

#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <span>
#include <string>
#include <string_view>
#include <vector>

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

  const BuildConfig config = emitMakefile(isDebug, /*includeDevDeps=*/false);
  outDir = config.outBasePath;

  const std::string& packageName = getPackageName();
  int exitCode = 0;
  if (config.isExecutable()) {
    const Command makeCmd = getMakeCommand().addArg("-C").addArg(outDir).addArg(
        (config.outBasePath / packageName).string()
    );
    Command checkUpToDateCmd = makeCmd;
    checkUpToDateCmd.addArg("--question");

    exitCode = execCmd(checkUpToDateCmd);
    if (exitCode != EXIT_SUCCESS) {
      // If packageName binary is not up-to-date, compile it.
      logger::info(
          "Compiling",

          "{} v{} ({})", packageName, getPackageVersion().toString(),
          getProjectBasePath().string()

      );
      exitCode = execCmd(makeCmd);
    }
  }

  if (config.isLibrary()) {
    std::string libName = fmt::format("lib{}.a", packageName);
    const Command makeCmd = getMakeCommand().addArg("-C").addArg(outDir).addArg(
        (config.outBasePath / libName).string()
    );
    Command checkUpToDateCmd = makeCmd;
    checkUpToDateCmd.addArg("--question");

    exitCode = execCmd(checkUpToDateCmd);
    if (exitCode != EXIT_SUCCESS) {
      // If packageName binary is not up-to-date, compile it.
      logger::info(
          "Compiling",
          fmt::format(
              "{} v{} ({})", libName, getPackageVersion().toString(),
              getProjectBasePath().string()
          )
      );
      exitCode = execCmd(makeCmd);
    }
  }

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;

  if (exitCode == EXIT_SUCCESS) {
    const Profile& profile = isDebug ? getDevProfile() : getReleaseProfile();

    std::vector<std::string_view> profiles;
    if (profile.optLevel.value() == 0) {
      profiles.emplace_back("unoptimized");
    } else {
      profiles.emplace_back("optimized");
    }
    if (profile.debug.value()) {
      profiles.emplace_back("debuginfo");
    }

    logger::info(
        "Finished", "`{}` profile [{}] target(s) in {:.2f}s",
        modeToProfile(isDebug), fmt::join(profiles, " + "), elapsed.count()
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
      ++itr;

      uint64_t numThreads{};
      auto [ptr, ec] =
          std::from_chars(itr->data(), itr->data() + itr->size(), numThreads);
      if (ec == std::errc()) {
        setParallelism(numThreads);
      } else {
        logger::error("invalid number of threads: ", *itr);
        return EXIT_FAILURE;
      }
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
  logger::info("Generated", "{}/compile_commands.json", outDir);
  return EXIT_SUCCESS;
}
