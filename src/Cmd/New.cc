#include "New.hpp"

#include "../Logger.hpp"

#include <fstream>
#include <iostream>

static inline constexpr StringRef mainCc =
    "#include <iostream>\n\n"
    "int main(int argc, char** argv) {\n"
    "  std::cout << \"Hello, world!\" << std::endl;\n"
    "}\n";

String getPoacToml(const String& projectName) {
  return
      "[package]\n"
      "name = \"" + projectName + "\"\n"
      "version = \"0.1.0\"\n"
      "authors = []\n"
      "edition = 2020\n";
}

static String getHeader(const String& projectName) {
  String projectNameUpperCased{};
  std::transform(
      projectName.cbegin(), projectName.cend(),
      std::back_inserter(projectNameUpperCased), ::toupper
  );

  return
      "#ifndef " + projectNameUpperCased + "_HPP\n"
      "#define " + projectNameUpperCased + "_HPP\n\n"
      "namespace " + projectName + " {\n}\n\n"
      "#endif // !" + projectNameUpperCased + "_HPP\n";
}

static void writeToFile(std::ofstream& ofs, const Path& fpath, StringRef text) {
  ofs.open(fpath);
  if (ofs.is_open()) {
    ofs << text;
  }
  ofs.close();
  ofs.clear();
}

static void createTemplateFiles(const bool isBin, const Path& projectName) {
  std::ofstream ofs;

  if (isBin) {
    fs::create_directories(projectName / "src");
    writeToFile(ofs, projectName / "poac.toml", getPoacToml(projectName));
    writeToFile(ofs, projectName / ".gitignore", "/poac-out");
    writeToFile(ofs, projectName / "src" / "main.cc", mainCc);

    Logger::status(
        "Created", "binary (application) `", projectName.string(), "` package"
    );
  } else {
    fs::create_directories(projectName / "include" / projectName);
    writeToFile(ofs, projectName / "poac.toml", getPoacToml(projectName));
    writeToFile(ofs, projectName / ".gitignore", "/poac-out\npoac.lock");
    writeToFile(
        ofs,
        (projectName / "include" / projectName / projectName).string() + ".hpp",
        getHeader(projectName)
    );

    Logger::status("Created", "library `", projectName.string(), "` package");
  }
}

bool verifyPackageName(StringRef name) {
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
      "alignas",
      "alignof",
      "and",
      "and_eq",
      "asm",
      "atomic_cancel",
      "atomic_commit",
      "atomic_noexcept",
      "auto",
      "bitand",
      "bitor",
      "bool",
      "break",
      "case",
      "catch",
      "char",
      "char8_t",
      "char16_t",
      "char32_t",
      "class",
      "compl",
      "concept",
      "const",
      "consteval",
      "constexpr",
      "constinit",
      "const_cast",
      "continue",
      "co_await",
      "co_return",
      "co_yield",
      "decltype",
      "default",
      "delete",
      "do",
      "double",
      "dynamic_cast",
      "else",
      "enum",
      "explicit",
      "export",
      "extern",
      "false",
      "float",
      "for",
      "friend",
      "goto",
      "if",
      "inline",
      "int",
      "long",
      "mutable",
      "namespace",
      "new",
      "noexcept",
      "not",
      "not_eq",
      "nullptr",
      "operator",
      "or",
      "or_eq",
      "private",
      "protected",
      "public",
      "reflexpr",
      "register",
      "reinterpret_cast",
      "requires",
      "return",
      "short",
      "signed",
      "sizeof",
      "static",
      "static_assert",
      "static_cast",
      "struct",
      "switch",
      "synchronized",
      "template",
      "this",
      "thread_local",
      "throw",
      "true",
      "try",
      "typedef",
      "typeid",
      "typename",
      "union",
      "unsigned",
      "using",
      "virtual",
      "void",
      "volatile",
      "wchar_t",
      "while",
      "xor",
      "xor_eq"
  };
  if (keywords.contains(name)) {
    Logger::error("package names cannot be a C++ keyword: ", name);
    return false;
  }

  return true;
}

int newMain(Vec<String> args) {
  if (args.empty()) {
    Logger::error("missing package name");
    return EXIT_FAILURE;
  }

  // Parse options
  bool isBin = true;
  String projectName;
  for (usize i = 0; i < args.size(); ++i) {
    String arg = args[i];
    if (arg == "--bin") {
      isBin = true;
    } else if (arg == "--lib") {
      isBin = false;
    } else {
      projectName = arg;
    }
  }

  if (!verifyPackageName(projectName)) {
    return EXIT_FAILURE;
  }

  createTemplateFiles(isBin, projectName);
  return EXIT_SUCCESS;
}

void newHelp() {
  std::cout << newDesc << '\n';
  std::cout << '\n';
  std::cout << "Usage: poac new [OPTIONS] <name>" << '\n';
  std::cout << '\n';
  std::cout << "Options:" << '\n';
  std::cout << "    --bin\t\t\tUse a binary (application) template [default]"
            << '\n';
  std::cout << "    --lib\t\t\tUse a library template" << '\n';
  std::cout << '\n';
  std::cout << "Args:" << '\n';
  std::cout << "    <name>\t\t\tName of the package" << '\n';
}
