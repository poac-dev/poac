#include "New.hpp"

#include "../Algos.hpp"
#include "../Cli.hpp"
#include "../Exception.hpp"
#include "../Git2.hpp"
#include "../Logger.hpp"
#include "../Manifest.hpp"
#include "../Rustify.hpp"
#include "Common.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <span>
#include <string>
#include <string_view>

static int newMain(std::span<const std::string_view> args);

const Subcmd NEW_CMD = //
    Subcmd{ "new" }
        .setDesc("Create a new poac project")
        .addOpt(OPT_BIN)
        .addOpt(OPT_LIB)
        .setArg(Arg{ "name" })
        .setMainFn(newMain);

static constexpr std::string_view MAIN_CC =
    "#include <iostream>\n\n"
    "int main() {\n"
    "  std::cout << \"Hello, world!\" << std::endl;\n"
    "  return 0;\n"
    "}\n";

static std::string
getAuthor() noexcept {
  try {
    git2::Config config = git2::Config();
    config.openDefault();
    return config.getString("user.name") + " <" + config.getString("user.email")
           + ">";
  } catch (const git2::Exception& e) {
    logger::debug("{}", e.what());
    return "";
  }
}

std::string
createPoacToml(const std::string_view projectName) noexcept {
  std::string poacToml =
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

static std::string
getHeader(const std::string_view projectName) noexcept {
  const std::string projectNameUpper = toMacroName(projectName);
  std::string header = "#ifndef " + projectNameUpper + "_HPP\n"
                  "#define " + projectNameUpper + "_HPP\n\n"
                  "namespace ";
  header += projectName;
  header +=
      " {\n}\n\n"
      "#endif  // !"
      + projectNameUpper + "_HPP\n";
  return header;
}

static void
writeToFile(
    std::ofstream& ofs, const fs::path& fpath, const std::string_view text
) {
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
createTemplateFiles(const bool isBin, const std::string_view projectName) {
  std::ofstream ofs;

  if (isBin) {
    fs::create_directories(projectName / "src"_path);
    writeToFile(
        ofs, projectName / "poac.toml"_path, createPoacToml(projectName)
    );
    writeToFile(ofs, projectName / ".gitignore"_path, "/poac-out");
    writeToFile(ofs, projectName / "src"_path / "main.cc", MAIN_CC);

    logger::info("Created", "binary (application) `", projectName, "` package");
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

    logger::info("Created", "library `", projectName, "` package");
  }
}

static int
newMain(const std::span<const std::string_view> args) {
  // Parse args
  bool isBin = true;
  std::string packageName;
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = Cli::handleGlobalOpts(itr, args.end(), "new")) {
      if (res.value() == Cli::CONTINUE) {
        continue;
      } else {
        return res.value();
      }
    } else if (*itr == "-b" || *itr == "--bin") {
      isBin = true;
    } else if (*itr == "-l" || *itr == "--lib") {
      isBin = false;
    } else if (packageName.empty()) {
      packageName = *itr;
    } else {
      return NEW_CMD.noSuchArg(*itr);
    }
  }

  if (const auto err = validatePackageName(packageName)) {
    logger::error("package names ", err.value(), ": `", packageName, '`');
    return EXIT_FAILURE;
  }

  if (fs::exists(packageName)) {
    logger::error("directory `", packageName, "` already exists");
    return EXIT_FAILURE;
  }

  createTemplateFiles(isBin, packageName);
  git2::Repository().init(packageName);
  return EXIT_SUCCESS;
}
