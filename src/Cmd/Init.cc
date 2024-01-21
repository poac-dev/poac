#include "Init.hpp"

#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "Global.hpp"
#include "New.hpp"

#include <cstdlib>
#include <fstream>
#include <span>
#include <string>

static constexpr auto initCli =
    Subcmd<2>("init")
        .setDesc(initDesc)
        .setUsage("[OPTIONS]")
        .addOpt(Opt{ "--bin", "-b" }.setDesc(
            "Use a binary (application) template [default]"
        ))
        .addOpt(Opt{ "--lib", "-l" }.setDesc("Use a library template"));

void
initHelp() noexcept {
  initCli.printHelp();
}

int
initMain(const std::span<const StringRef> args) {
  // Parse args
  bool isBin = true;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "init" } })

    else if (arg == "-b" || arg == "--bin") {
      isBin = true;
    }
    else if (arg == "-l" || arg == "--lib") {
      isBin = false;
    }
    else {
      return initCli.noSuchArg(arg);
    }
  }

  if (fs::exists("poac.toml")) {
    Logger::error("cannot initialize an existing poac package");
    return EXIT_FAILURE;
  }

  const String packageName = fs::current_path().stem().string();
  if (const auto err = validatePackageName(packageName)) {
    Logger::error("package names ", err.value(), ": `", packageName, '`');
    return EXIT_FAILURE;
  }

  std::ofstream ofs("poac.toml");
  ofs << createPoacToml(packageName);

  Logger::info(
      "Created", isBin ? "binary (application) `" : "library `", packageName,
      "` package"
  );
  return EXIT_SUCCESS;
}
