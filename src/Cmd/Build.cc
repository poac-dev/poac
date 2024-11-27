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

BuildSystem
getDefaultBuildSystem() {
#ifdef _WIN32
  return BuildSystem::Xmake;
#else
  return BuildSystem::Makefile;
#endif
}

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
        .addOpt(Opt{ "--build-system" }
                    .setDesc("Specify build system (makefile/xmake)")
                    .setPlaceholder("SYSTEM"))
        .setMainFn(buildMain);

int
buildImpl(std::string& outDir, const bool isDebug, BuildSystem buildSystem) {
  const auto start = std::chrono::steady_clock::now();

  const BuildConfig config = emitBuildFiles(buildSystem, isDebug, false);
  outDir = config.outBasePath.string();

  const std::string& packageName = getPackageName();
  Command buildCmd = getBuildCommand(buildSystem);

  auto logBuildCompletion = [&](int exitCode,
                                const std::chrono::duration<double>& elapsed) {
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
  };

  int exitCode = EXIT_FAILURE;
  switch (buildSystem) {
    case BuildSystem::Makefile: {
      buildCmd.addArg("-C").addArg(outDir).addArg(
          (config.outBasePath / packageName).string()
      );
      Command checkUpToDateCmd = buildCmd;
      checkUpToDateCmd.addArg("--question");

      exitCode = execCmd(checkUpToDateCmd);
      if (exitCode != EXIT_SUCCESS) {
        logger::info(
            "Compiling", "{} v{} ({})", packageName,
            getPackageVersion().toString(), getProjectBasePath().string()
        );
        exitCode = execCmd(buildCmd);
      }
      break;
    }
    case BuildSystem::Xmake: {
      Command configCmd = buildCmd;
      auto xmakePath = (config.outBasePath / "xmake.lua").string();
      configCmd.addArg("f")
          .addArg("-m")
          .addArg(isDebug ? "debug" : "release")
          .addArg("-F")
          .addArg(xmakePath);
      exitCode = execCmd(configCmd);
      if (exitCode == EXIT_SUCCESS) {
        buildCmd.addArg("-F").addArg(xmakePath);
        logger::info(
            "Compiling", "{} v{} ({})", packageName,
            getPackageVersion().toString(), getProjectBasePath().string()
        );
        exitCode = execCmd(buildCmd);
      }
      break;
    }
  }

  const auto end = std::chrono::steady_clock::now();
  const std::chrono::duration<double> elapsed = end - start;
  logBuildCompletion(exitCode, elapsed);

  return exitCode;
}

static int
buildMain(const std::span<const std::string_view> args) {
  // Parse args
  bool isDebug = true;
  bool buildCompdb = false;
  BuildSystem buildSystem = getDefaultBuildSystem();

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
    } else if (*itr == "--build-system") {
      if (itr + 1 == args.end()) {
        return Subcmd::missingArgumentForOpt(*itr);
      }
      ++itr;
      if (*itr == "makefile") {
        buildSystem = BuildSystem::Makefile;
      } else if (*itr == "xmake") {
        buildSystem = BuildSystem::Xmake;
      } else {
        logger::error("unsupported build system: ", *itr);
        return EXIT_FAILURE;
      }
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
    return buildImpl(outDir, isDebug, buildSystem);
  }

  // Build compilation database
  const std::string outDir = emitCompdb(isDebug, /*includeDevDeps=*/false);
  logger::info("Generated", "{}/compile_commands.json", outDir);
  return EXIT_SUCCESS;
}
