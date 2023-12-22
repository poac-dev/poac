#include "Fmt.hpp"

#include "../Algos.hpp"
#include "../BuildConfig.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Rustify.hpp"
#include "Global.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <span>

int fmtMain(std::span<const StringRef> args) {
  bool isCheck = false;
  // Parse args
  for (StringRef arg : args) {
    HANDLE_GLOBAL_OPTS({{"fmt"}})

    else if (arg == "--check") {
      isCheck = true;
    }
    else {
      Logger::error("invalid argument: ", arg);
      return EXIT_FAILURE;
    }
  }

  if (!commandExists("clang-format")) {
    Logger::error(
        "fmt command requires clang-format; try installing it by:\n"
        "  apt/brew install clang-format"
    );
    return EXIT_FAILURE;
  }

  const String packageName = getPackageName();
  String clangFormatArgs = "--style=file --fallback-style=LLVM -Werror";
  if (isVerbose()) {
    clangFormatArgs += " --verbose";
  }
  if (isCheck) {
    clangFormatArgs += " --dry-run";
  } else {
    clangFormatArgs += " -i";
    Logger::info("Formatting", packageName);
  }

  // Read .gitignore if exists
  TrieNode root;
  if (fs::exists(".gitignore")) {
    std::ifstream ifs(".gitignore");
    String line;
    while (std::getline(ifs, line)) {
      if (line.empty() || line[0] == '#') {
        continue;
      }

      trieInsert(root, line);
    }
  }

  // Automatically collects format-target files
  for (auto entry = fs::recursive_directory_iterator(".");
       entry != fs::recursive_directory_iterator(); ++entry) {
    if (entry->is_directory()) {
      const String path = entry->path().string();
      if (trieSearchFromAnyPosition(root, path)) {
        Logger::debug("ignore: ", path);
        entry.disable_recursion_pending();
        continue;
      }
    } else if (entry->is_regular_file()) {
      const Path path = entry->path();
      if (trieSearchFromAnyPosition(root, path.string())) {
        Logger::debug("Ignore: ", path.string());
        continue;
      }

      const String ext = path.extension().string();
      if (SOURCE_FILE_EXTS.contains(ext) || HEADER_FILE_EXTS.contains(ext)) {
        clangFormatArgs += " " + path.string();
      }
    }
  }

  const String clangFormat = "clang-format " + clangFormatArgs;
  Logger::debug("Executing `", clangFormat, '`');
  const int code = std::system(clangFormat.c_str());
  const int exitCode = code >> 8;
  if (exitCode != 0) {
    Logger::error("clang-format exited with code ", exitCode);
    return exitCode;
  }
  return EXIT_SUCCESS;
}

void fmtHelp() noexcept {
  std::cout << fmtDesc << '\n';
  std::cout << '\n';
  printUsage("fmt", "[OPTIONS]");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption("--check", "", "Run clang-format in check mode");
}
