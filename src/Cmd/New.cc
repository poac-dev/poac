#include "New.hpp"

#include "../Exception.hpp"
#include "../Git/Git2.hpp"
#include "../Logger.hpp"
#include "../Rustify.hpp"
#include "Global.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <span>
#include <string>

static inline constexpr StringRef mainCc =
    "#include <iostream>\n\n"
    "int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {\n"
    "  std::cout << \"Hello, world!\" << std::endl;\n"
    "}\n";

String getPoacToml(StringRef projectName) noexcept {
  String poacToml =
      "[package]\n"
      "name = \"";
  poacToml += projectName;
  poacToml +=
      "\"\n"
      "version = \"0.1.0\"\n"
      "authors = []\n"
      "edition = \"20\"\n";
  return poacToml;
}

static String getHeader(StringRef projectName) noexcept {
  String projectNameUpper{};
  std::transform(
      projectName.cbegin(), projectName.cend(),
      std::back_inserter(projectNameUpper), ::toupper
  );

  String header = "#ifndef " + projectNameUpper + "_HPP\n"
                  "#define " + projectNameUpper + "_HPP\n\n"
                  "namespace ";
  header += projectName;
  header +=
      " {\n}\n\n"
      "#endif // !"
      + projectNameUpper + "_HPP\n";
  return header;
}

static void writeToFile(std::ofstream& ofs, const Path& fpath, StringRef text) {
  ofs.open(fpath);
  if (ofs.is_open()) {
    ofs << text;
  }
  ofs.close();

  if (!ofs) {
    throw PoacError("writing `", fpath.string(), "` failed");
  }
  ofs.clear();
}

static void createTemplateFiles(const bool isBin, StringRef projectName) {
  std::ofstream ofs;

  if (isBin) {
    fs::create_directories(projectName / "src"_path);
    writeToFile(ofs, projectName / "poac.toml"_path, getPoacToml(projectName));
    writeToFile(ofs, projectName / ".gitignore"_path, "/poac-out");
    writeToFile(ofs, projectName / "src"_path / "main.cc", mainCc);

    Logger::info("Created", "binary (application) `", projectName, "` package");
  } else {
    fs::create_directories(projectName / "include"_path / projectName);
    writeToFile(ofs, projectName / "poac.toml"_path, getPoacToml(projectName));
    writeToFile(ofs, projectName / ".gitignore"_path, "/poac-out\npoac.lock");
    writeToFile(
        ofs,
        (projectName / "include"_path / projectName / projectName).string()
            + ".hpp",
        getHeader(projectName)
    );

    Logger::info("Created", "library `", projectName, "` package");
  }
}

bool verifyPackageName(StringRef name) noexcept {
  // Empty
  if (name.empty()) {
    Logger::error("missing package name");
    return false;
  }

  // Only one character
  if (name.size() == 1) {
    Logger::error("only one character in package name: ", name);
    return false;
  }

  // Only lowercase letters, numbers, dashes, and underscores
  for (const char& c : name) {
    if (!std::islower(c) && !std::isdigit(c) && c != '-' && c != '_') {
      Logger::error("invalid character in package name: ", name);
      return false;
    }
  }

  // Start with a letter
  if (!std::isalpha(name[0])) {
    Logger::error("package names must start with a letter: ", name);
    return false;
  }

  // End with a letter or digit
  if (!std::isalnum(name[name.size() - 1])) {
    Logger::error("package names must end with a letter or digit: ", name);
    return false;
  }

  // Using C++ keywords
  const HashSet<StringRef> keywords = {
#include "../Keywords.def"
  };
  if (keywords.contains(name)) {
    Logger::error("package names cannot be a C++ keyword: ", name);
    return false;
  }

  return true;
}

int newMain(std::span<const StringRef> args) {
  // Parse args
  bool isBin = true;
  String packageName;
  for (usize i = 0; i < args.size(); ++i) {
    StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "new" } })

    else if (arg == "-b" || arg == "--bin") {
      isBin = true;
    }
    else if (arg == "-l" || arg == "--lib") {
      isBin = false;
    }
    else if (packageName.empty()) {
      packageName = arg;
    }
    else {
      Logger::error("too many arguments: ", arg);
      return EXIT_FAILURE;
    }
  }

  if (!verifyPackageName(packageName)) {
    return EXIT_FAILURE;
  }

  if (fs::exists(packageName)) {
    Logger::error("directory `", packageName, "` already exists");
    return EXIT_FAILURE;
  }

  createTemplateFiles(isBin, packageName);
  git2::repository().init(packageName);
  return EXIT_SUCCESS;
}

void newHelp() noexcept {
  std::cout << newDesc << '\n';
  std::cout << '\n';
  printUsage("new", "[OPTIONS] <name>");
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  printOption("--bin", "-b", "Use a binary (application) template [default]");
  printOption("--lib", "-l", "Use a library template");
  std::cout << '\n';
  printHeader("Arguments:");
  std::cout << "  <name>" << '\n';
}
