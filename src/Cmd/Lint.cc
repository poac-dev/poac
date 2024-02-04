#include "Lint.hpp"

#include "../Algos.hpp"
#include "../Cli.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Rustify.hpp"

#include <cstdlib>
#include <fstream>
#include <span>

static int lintMain(std::span<const StringRef> args);

const Subcmd LINT_CMD =
    Subcmd{ "lint" }
        .setDesc("Lint codes using cpplint")
        .addOpt(Opt{ "--exclude" }.setDesc("Exclude files from linting"))
        .setMainFn(lintMain);

struct LintArgs {
  String excludes;
};

static int
lint(const StringRef name, const StringRef cpplintArgs) {
  logger::info("Linting", name);

  String cpplintCmd = "cpplint";
  cpplintCmd += cpplintArgs;
  if (!isVerbose()) {
    cpplintCmd += " --quiet";
  }

  // Read .gitignore if exists
  if (fs::exists(".gitignore")) {
    std::ifstream ifs(".gitignore");
    String line;
    while (std::getline(ifs, line)) {
      if (line.empty() || line[0] == '#') {
        continue;
      }

      cpplintCmd += " --exclude=";
      cpplintCmd += line;
    }
  }
  // NOTE: This should come after the `--exclude` options.
  cpplintCmd += " --recursive .";
  return execCmd(cpplintCmd);
}

static int
lintMain(const std::span<const StringRef> args) {
  LintArgs lintArgs;
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Cli::handleGlobalOpts(itr, args.end(), "lint")) {
      if (res.value() == Cli::CONTINUE) {
        continue;
      } else {
        return res.value();
      }
    } else if (*itr == "--exclude") {
      if (itr + 1 >= args.end()) {
        logger::error("Missing argument for ", *itr);
        return EXIT_FAILURE;
      }

      lintArgs.excludes += " --exclude=";
      lintArgs.excludes += *++itr;
    } else {
      return LINT_CMD.noSuchArg(*itr);
    }
  }

  if (!commandExists("cpplint")) {
    logger::error(
        "lint command requires cpplint; try installing it by:\n"
        "  pip install cpplint"
    );
    return EXIT_FAILURE;
  }

  String cpplintArgs = lintArgs.excludes;
  const StringRef packageName = getPackageName();
  if (fs::exists("CPPLINT.cfg")) {
    logger::debug("Using CPPLINT.cfg for lint ...");
    return lint(packageName, cpplintArgs);
  }

  if (fs::exists("include")) {
    cpplintArgs += " --root=include";
  } else if (fs::exists("src")) {
    cpplintArgs += " --root=src";
  }

  const Vec<String>& cpplintFilters = getLintCpplintFilters();
  if (!cpplintFilters.empty()) {
    logger::debug("Using Poac manifest file for lint ...");
    cpplintArgs += " --filter=";
    for (const StringRef filter : cpplintFilters) {
      cpplintArgs += filter;
      cpplintArgs += ',';
    }
    // Remove last comma
    cpplintArgs.pop_back();
    return lint(packageName, cpplintArgs);
  } else {
    logger::debug("Using default arguments for lint ...");
    if (Edition::Cpp11 < getPackageEdition()) {
      // Disable C++11-related lints
      cpplintArgs += " --filter=-build/c++11";
    }
    return lint(packageName, cpplintArgs);
  }
}
