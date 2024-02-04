#include "Clean.hpp"

#include "../Cli.hpp"
#include "../Logger.hpp"

#include <cstdlib>
#include <span>
#include <string>

static int cleanMain(std::span<const StringRef> args) noexcept;

const Subcmd CLEAN_CMD = //
    Subcmd{ "clean" }
        .setDesc("Remove the built directory")
        .addOpt(Opt{ "--profile" }
                    .setShort("-p")
                    .setDesc("Disable parallel builds")
                    .setPlaceholder("<PROFILE>"))
        .setMainFn(cleanMain);

static int
cleanMain(const std::span<const StringRef> args) noexcept {
  Path outDir = "poac-out"; // TODO: share across sources

  // Parse args
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Command::handleGlobalOpts(itr, args.end(), "clean")) {
      return res.value();
    } else if (*itr == "-p"sv || *itr == "--profile") {
      if (itr + 1 >= args.end()) {
        logger::error("Missing argument for ", *itr);
        return EXIT_FAILURE;
      }

      ++itr;
      if (!(*itr == "debug" || *itr == "release")) {
        logger::error("Invalid argument for ", *(itr - 1), ": ", *itr);
        return EXIT_FAILURE;
      }

      outDir /= *itr;
    } else {
      return CLEAN_CMD.noSuchArg(*itr);
    }
  }

  if (fs::exists(outDir)) {
    logger::info("Removing", fs::canonical(outDir).string());
    fs::remove_all(outDir);
  }
  return EXIT_SUCCESS;
}
