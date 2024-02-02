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
  Path outDir = "poac-out";

  // Parse args
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "clean" } })

    else if (arg == "-p" || arg == "--profile") {
      if (i + 1 >= args.size()) {
        log::error("Missing argument for ", arg);
        return EXIT_FAILURE;
      }

      ++i;

      if (!(args[i] == "debug" || args[i] == "release")) {
        log::error("Invalid argument for ", arg, ": ", args[i]);
        return EXIT_FAILURE;
      }

      outDir /= args[i];
    }
    else {
      return CLEAN_CMD.noSuchArg(arg);
    }
  }

  if (fs::exists(outDir)) {
    log::info("Removing", fs::canonical(outDir).string());
    fs::remove_all(outDir);
  }
  return EXIT_SUCCESS;
}
