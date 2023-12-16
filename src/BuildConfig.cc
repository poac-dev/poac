#include "BuildConfig.hpp"

#include "Algos.hpp"
#include "Logger.hpp"

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

    if (!sortedVars.empty() && !targets.empty()) {
      os << '\n';
    }

    const Vec<String> sortedTargets = topoSort(targets, targetDeps);
    for (auto itr = sortedTargets.rbegin(); itr != sortedTargets.rend();
         itr++) {
      os << *itr << ":";
      for (const auto& dep : targets.at(*itr).dependsOn) {
        os << " " << dep;
      }
      os << '\n';

      for (const auto& cmd : targets.at(*itr).commands) {
        os << '\t' << cmd << '\n';
      }
      os << '\n';
    }
  }
};

static Vec<String> listSourceFiles(const String& directory) {
  Vec<String> sourceFiles;
  for (const auto& entry : fs::recursive_directory_iterator(directory)) {
    if (SOURCE_FILE_EXTS.count(entry.path().extension()) == 0) {
      continue;
    }
    sourceFiles.push_back(entry.path().string());
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

static bool isMakefileUpToDate(const String& makefilePath) {
  if (!fs::exists(makefilePath)) {
    return false;
  }

  const fs::file_time_type makefileTime = fs::last_write_time(makefilePath);
  // Makefile depends on all files in ./src and poac.toml.
  for (const auto& entry : fs::recursive_directory_iterator("src")) {
    if (fs::last_write_time(entry.path()) > makefileTime) {
      return false;
    }
  }
  if (fs::last_write_time("poac.toml") > makefileTime) {
    return false;
  }

  return true;
}

static bool containsTestCode(const String& sourceFile) {
  std::ifstream ifs(sourceFile);
  String line;
  while (std::getline(ifs, line)) {
    if (line.find("POAC_TEST") != String::npos) {
      return true;
    }
  }
  return false;
}

static void addDirectoryTarget(BuildConfig& config, const String& directory) {
  config.defineTarget(directory, {"mkdir -p $@"});
}

// Returns the directory where the Makefile is generated.
String emitMakefile(const Vec<String>& args) {
  if (!fs::exists("src")) {
    throw std::runtime_error("src directory not found");
  }
  if (!fs::exists("src/main.cc")) {
    throw std::runtime_error("src/main.cc not found");
  }

  bool debug = true;
  if (!args.empty()) {
    if (args[0] == "-d" || args[0] == "--debug") {
      // Do nothing
    } else if (args[0] == "-r" || args[0] == "--release") {
      debug = false;
      OUT_DIR = "poac-out/release";
    } else {
      throw std::runtime_error(
          "invalid option: `" + args[0] + "`\n\n"
          "       run `poac help build` for a list of options"
      );
    }
  }
  if (!fs::exists(OUT_DIR)) {
    fs::create_directories(OUT_DIR);
  }

  const String makefilePath = OUT_DIR + "/Makefile";
  if (isMakefileUpToDate(makefilePath)) {
    Logger::debug("Makefile is up to date");
    return OUT_DIR;
  }

  const String projectName = "poac";
  const String pathFromOutDir = "../../";

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

  // Build rules
  config.defineTarget(".PHONY", {}, {"all"});

  const String buildOutDir = projectName + ".d";
  addDirectoryTarget(config, buildOutDir);

  config.defineTarget("all", {}, {buildOutDir, projectName});

  const Vec<String> sourceFiles = listSourceFiles("src");
  Vec<String> objectFiles;
  for (String sourceFile : sourceFiles) {
    sourceFile = pathFromOutDir + sourceFile;
    const String mmOutput = runMM(sourceFile);

    String target;
    Vec<String> dependencies;
    parseMMOutput(mmOutput, target, dependencies);

    // if (containsTestCode(sourceFile)) {
    //   addDirectoryTarget(config, "tests");
    // }

    const String targetBaseDir =
        fs::relative(Path(sourceFile).parent_path(), pathFromOutDir + "src")
            .string();
    target = buildOutDir + "/" + targetBaseDir + "/" + target;

    // Add a target to create the targetBaseDir.
    if (targetBaseDir != ".") {
      addDirectoryTarget(config, buildOutDir + "/" + targetBaseDir);
      dependencies.push_back(buildOutDir + "/" + targetBaseDir);
    }

    objectFiles.push_back(target);
    config.defineTarget(target, {"$(CC) $(CFLAGS) -c $< -o $@"}, dependencies);
  }

  // The project binary
  config.defineTarget(projectName, {"$(CC) $(CFLAGS) $^ -o $@"}, objectFiles);

  std::ofstream ofs(makefilePath);
  config.emitMakefile(ofs);
  return OUT_DIR;
}

#ifdef POAC_TEST

#  include <cassert>
#  include <sstream>
#  include <stdexcept>

void test_cycle_vars() {
  BuildConfig config;
  config.defineVariable("a", "b", {"b"});
  config.defineVariable("b", "c", {"c"});
  config.defineVariable("c", "a", {"a"});

  try {
    std::stringstream ss;
    config.emitMakefile(ss);
  } catch (const std::runtime_error& e) {
    assert(std::string(e.what()) == "too complex build graph");
    return;
  }

  assert(false && "should not reach here");
}

void test_simple_vars() {
  BuildConfig config;
  config.defineVariable("c", "3", {"b"});
  config.defineVariable("b", "2", {"a"});
  config.defineVariable("a", "1");

  std::stringstream ss;
  config.emitMakefile(ss);

  assert(ss.str() == "a = 1\n"
                      "b = 2\n"
                      "c = 3\n");
}

void test_depend_on_unregistered_var() {
  BuildConfig config;
  config.defineVariable("a", "1", {"b"});

  std::stringstream ss;
  config.emitMakefile(ss);

  assert(ss.str() == "a = 1\n");
}

void test_cycle_targets() {
  BuildConfig config;
  config.defineTarget("a", {"echo a"}, {"b"});
  config.defineTarget("b", {"echo b"}, {"c"});
  config.defineTarget("c", {"echo c"}, {"a"});

  try {
    std::stringstream ss;
    config.emitMakefile(ss);
  } catch (const std::runtime_error& e) {
    assert(std::string(e.what()) == "too complex build graph");
    return;
  }

  assert(false && "should not reach here");
}

void test_simple_targets() {
  BuildConfig config;
  config.defineTarget("a", {"echo a"});
  config.defineTarget("b", {"echo b"}, {"a"});
  config.defineTarget("c", {"echo c"}, {"b"});

  std::stringstream ss;
  config.emitMakefile(ss);

  assert(ss.str() == "c: b\n"
                      "\techo c\n"
                      "\n"
                      "b: a\n"
                      "\techo b\n"
                      "\n"
                      "a:\n"
                      "\techo a\n"
                      "\n");
}

void test_depend_on_unregistered_target() {
  BuildConfig config;
  config.defineTarget("a", {"echo a"}, {"b"});

  std::stringstream ss;
  config.emitMakefile(ss);

  assert(ss.str() == "a: b\n"
                      "\techo a\n"
                      "\n");
}

int main() {
  test_cycle_vars();
  test_simple_vars();
  test_depend_on_unregistered_var();
  test_cycle_targets();
  test_simple_targets();
  test_depend_on_unregistered_target();
}
#endif
