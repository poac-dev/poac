#include "Init.hpp"

#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "Common.hpp"
#include "New.hpp"

#include <cstdlib>
#include <fstream>
#include <span>
#include <string>

static int initMain(std::span<const StringRef> args);

const Subcmd INIT_CMD =
    Subcmd{ "init" }
        .setDesc("Create a new poac package in an existing directory")
        .addOpt(OPT_BIN)
        .addOpt(OPT_LIB)
        .setMainFn(initMain);

static int
initMain(const std::span<const StringRef> args) {
  // Parse args
  bool isBin = true;
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Cli::handleGlobalOpts(itr, args.end(), "init")) {
      if (res.value() == Cli::CONTINUE) {
        continue;
      } else {
        return res.value();
      }
    } else if (*itr == "-b" || *itr == "--bin") {
      isBin = true;
    } else if (*itr == "-l" || *itr == "--lib") {
      isBin = false;
    } else {
      return INIT_CMD.noSuchArg(*itr);
    }
  }

  if (fs::exists("poac.toml")) {
    logger::error("cannot initialize an existing poac package");
    return EXIT_FAILURE;
  }

  const std::string packageName = fs::current_path().stem().string();
  if (const auto err = validatePackageName(packageName)) {
    logger::error("package names ", err.value(), ": `", packageName, '`');
    return EXIT_FAILURE;
  }

  std::ofstream ofs("poac.toml");
  ofs << createPoacToml(packageName);

  logger::info(
      "Created", isBin ? "binary (application) `" : "library `", packageName,
      "` package"
  );
  return EXIT_SUCCESS;
}
