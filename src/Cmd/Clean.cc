#include "Clean.hpp"

#include "../Logger.hpp"
#include "Global.hpp"

#include <iostream>
#include <span>
#include <string>

int cleanMain(std::span<const StringRef> args) noexcept {
  Path outDir = "poac-out";

  // Parse args
  for (usize i = 0; i < args.size(); ++i) {
    StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({{"clean"}})

    else if (arg == "-p" || arg == "--profile") {
      if (i + 1 >= args.size()) {
        Logger::error("Missing argument for ", arg);
        return EXIT_FAILURE;
      }

      ++i;

      if (!(args[i] == "debug" || args[i] == "release")) {
        Logger::error("Invalid argument for ", arg, ": ", args[i]);
        return EXIT_FAILURE;
      }

      outDir /= args[1];
    }
    else {
      Logger::error("Unknown argument: ", arg);
      return EXIT_FAILURE;
    }
  }

  if (fs::exists(outDir)) {
    Logger::info("Removing", fs::canonical(outDir).string());
    fs::remove_all(outDir);
  }
  return EXIT_SUCCESS;
}

void cleanHelp() noexcept {
  std::cout << cleanDesc << '\n';
  std::cout << '\n';
  printUsage("clean", "[OPTIONS]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption(
      "--profile", "-p", "Remove built artifacts in <PROFILE> mode", "<PROFILE>"
  );
}
