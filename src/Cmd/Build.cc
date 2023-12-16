#include "Build.hpp"

#include "../Algos.hpp"
#include "../Logger.hpp"
#include "../Rustify.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>

static String OUT_DIR = "poac-out/debug";

struct Target {
  Vec<String> commands;
  Vec<String> dependsOn;
};

struct BuildConfig {
  HashMap<String, String> variables;
  HashMap<String, Vec<String>> varDeps;
  HashMap<String, Target> targets;
  HashMap<String, Vec<String>> targetDeps;

  void defineVariable(String name, String value, Vec<String> dependsOn = {}) {
    variables[name] = value;
    for (const String& dep : dependsOn) {
      // reverse dependency
      varDeps[dep].push_back(name);
    }
  }

  void defineTarget(
      String name, const Vec<String>& commands, Vec<String> dependsOn = {}
  ) {
    targets[name] = {commands, dependsOn};
    for (const String& dep : dependsOn) {
      // reverse dependency
      targetDeps[dep].push_back(name);
    }
  }

  void emitMakefile(std::ostream& os = std::cout) const {
    const Vec<String> sortedVars = topoSort(variables, varDeps);
    for (const auto& var : sortedVars) {
      os << var << " = " << variables.at(var) << '\n';
    }
    os << '\n';

    const Vec<String> sortedTargets = topoSort(targets, targetDeps);
    for (auto itr = sortedTargets.rbegin(); itr != sortedTargets.rend();
         itr++) {
      os << *itr << ": ";
      for (const auto& dep : targets.at(*itr).dependsOn) {
        os << dep << " ";
      }
      os << '\n';

      for (const auto& cmd : targets.at(*itr).commands) {
        os << "\t" << cmd << '\n';
      }
      os << '\n';
    }
  }
};

static Vec<String> listSourceFiles(const String& directory) {
  Vec<String> sourceFiles;
  for (const auto& entry :
       std::filesystem::recursive_directory_iterator(directory)) {
    if (entry.path().extension() == ".cc") {
      sourceFiles.push_back(entry.path().string());
    }
  }
  return sourceFiles;
}

static String exec(const char* cmd) {
  std::array<char, 128> buffer;
  String result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

static String runMM(const String& sourceFile) {
  const String command = "cd " + OUT_DIR + " && clang++ -MM " + sourceFile;
  return exec(command.c_str());
}

static void parseMMOutput(
    const String& mmOutput, String& target, Vec<String>& dependencies
) {
  std::istringstream iss(mmOutput);
  std::getline(iss, target, ':');
  Logger::debug(target, ':');

  String dependency;
  while (std::getline(iss, dependency, ' ')) {
    if (!dependency.empty() && dependency.front() != '\\') {
      // Remove trailing newline if it exists
      if (dependency.back() == '\n') {
        dependency.pop_back();
      }
      dependencies.push_back(dependency);
      Logger::debug(" '", dependency, "'");
    }
  }
  Logger::debug("");
}

int build(Vec<String> args) {
  if (!std::filesystem::exists("src")) {
    throw std::runtime_error("src directory not found");
  }
  if (!std::filesystem::exists("src/main.cc")) {
    throw std::runtime_error("src/main.cc not found");
  }

  bool debug = true;
  if (!args.empty()) {
    if (args[0] == "-d" || args[0] == "--debug") {
      debug = true;
    } else if (args[0] == "-r" || args[0] == "--release") {
      debug = false;
      OUT_DIR = "poac-out/release";
    } else {
      Logger::error(
          "invalid option: `", args[0], "`", "\n\n",
          "       run `poac help build` for a list of options"
      );
      return EXIT_FAILURE;
    }
  }
  if (!std::filesystem::exists(OUT_DIR)) {
    std::filesystem::create_directories(OUT_DIR);
  }

  BuildConfig config;

  // Compiler settings
  config.defineVariable("CC", "clang++");
  const String baseCflags =
      "-Wall -Wextra -fdiagnostics-color -pedantic-errors -std=c++20 ";
  if (debug) {
    config.defineVariable("CFLAGS", baseCflags + "-g -O0 -DDEBUG");
  } else {
    config.defineVariable("CFLAGS", baseCflags + "-O3 -DNDEBUG");
  }
  config.defineVariable("LDFLAGS", "-L.");
  // Directories
  config.defineVariable("SRC_DIR", "../../src"); // poac-out/debug
  // Project settings
  config.defineVariable("PROJ_NAME", "poac");
  config.defineVariable("MAIN", "$(SRC_DIR)/main.cc", {"SRC_DIR"});

  // Build rules
  config.defineTarget(".PHONY", {}, {"all"});
  config.defineTarget("all", {}, {"$(PROJ_NAME)"});

  const Vec<String> sourceFiles = listSourceFiles("src");
  Vec<String> objectFiles;
  for (String sourceFile : sourceFiles) {
    sourceFile = "../../" + sourceFile;
    String mmOutput = runMM(sourceFile);

    String target;
    Vec<String> dependencies;
    parseMMOutput(mmOutput, target, dependencies);

    objectFiles.push_back(target);
    config.defineTarget(
        target, {"$(CC) $(CFLAGS) -c " + sourceFile + " -o $@"}, dependencies
    );
  }

  // The project binary
  config.defineTarget(
      "$(PROJ_NAME)", {"$(CC) $(CFLAGS) $^ -o $@"}, objectFiles
  );

  std::ofstream ofs(OUT_DIR + "/Makefile");
  config.emitMakefile(ofs);
  ofs.close();

  std::system(("cd " + OUT_DIR + " && make").c_str());
  return EXIT_SUCCESS;
}

void build_help() {
  std::cout << "poac-build" << '\n';
  std::cout << '\n';
  std::cout << "USAGE:" << '\n';
  std::cout << "    poac build [OPTIONS]" << '\n';
  std::cout << '\n';
  std::cout << "OPTIONS:" << '\n';
  std::cout << "    -d, --debug\t\tBuild with debug information (default)"
            << '\n';
  std::cout << "    -r, --release\tBuild with optimizations" << '\n';
}
