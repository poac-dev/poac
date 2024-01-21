#include "New.hpp"

#include "../Algos.hpp"
#include "../Exception.hpp"
#include "../Git2.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Rustify.hpp"
#include "Global.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <span>
#include <string>

static constexpr auto NEW_CLI =
    Subcmd<2>("new")
        .setDesc(newDesc)
        .addOpt(Opt{ "--bin", "-b" }.setDesc(
            "Use a binary (application) template [default]"
        ))
        .addOpt(Opt{ "--lib", "-l" }.setDesc("Use a library template"))
        .setArg(Arg{ "<name>" });

void
newHelp() noexcept {
  NEW_CLI.printHelp();
}

static inline constexpr StringRef MAIN_CC =
    "#include <iostream>\n\n"
    "int main() {\n"
    "  std::cout << \"Hello, world!\" << std::endl;\n"
    "}\n";

static String
getAuthor() noexcept {
  try {
    git2::Config config = git2::Config();
    config.openDefault();
    return config.getString("user.name") + " <" + config.getString("user.email")
           + ">";
  } catch (const git2::Exception& e) {
    Logger::debug(e.what());
    return "";
  }
}

String
createPoacToml(const StringRef projectName) noexcept {
  String poacToml =
      "[package]\n"
      "name = \"";
  poacToml += projectName;
  poacToml +=
      "\"\n"
      "version = \"0.1.0\"\n"
      "authors = [\"";
  poacToml += getAuthor();
  poacToml +=
      "\"]\n"
      "edition = \"20\"\n";
  return poacToml;
}

static String
getHeader(const StringRef projectName) noexcept {
  const String projectNameUpper = toMacroName(projectName);
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

static void
writeToFile(std::ofstream& ofs, const Path& fpath, const StringRef text) {
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

static void
createTemplateFiles(const bool isBin, const StringRef projectName) {
  std::ofstream ofs;

  if (isBin) {
    fs::create_directories(projectName / "src"_path);
    writeToFile(
        ofs, projectName / "poac.toml"_path, createPoacToml(projectName)
    );
    writeToFile(ofs, projectName / ".gitignore"_path, "/poac-out");
    writeToFile(ofs, projectName / "src"_path / "main.cc", MAIN_CC);

    Logger::info("Created", "binary (application) `", projectName, "` package");
  } else {
    fs::create_directories(projectName / "include"_path / projectName);
    writeToFile(
        ofs, projectName / "poac.toml"_path, createPoacToml(projectName)
    );
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

int
newMain(const std::span<const StringRef> args) {
  // Parse args
  bool isBin = true;
  String packageName;
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
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
      return NEW_CLI.noSuchArg(arg);
    }
  }

  if (const auto err = validatePackageName(packageName)) {
    Logger::error("package names ", err.value(), ": `", packageName, '`');
    return EXIT_FAILURE;
  }

  if (fs::exists(packageName)) {
    Logger::error("directory `", packageName, "` already exists");
    return EXIT_FAILURE;
  }

  createTemplateFiles(isBin, packageName);
  git2::Repository().init(packageName);
  return EXIT_SUCCESS;
}
