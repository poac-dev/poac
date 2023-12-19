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
static String CXX = "clang++";

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
    for (const String& var : sortedVars) {
      if (var == "CXX") {
        os << var << " ?= " << variables.at(var) << '\n';
      } else {
        os << var << " = " << variables.at(var) << '\n';
      }
    }

    if (!sortedVars.empty() && !targets.empty()) {
      os << '\n';
    }

    if (targets.count(".PHONY") != 0) {
      os << ".PHONY:";
      for (const auto& dep : targets.at(".PHONY").dependsOn) {
        os << " " << dep;
      }
      os << "\n\n";
    }
    if (targets.count("all") != 0) {
      os << "all:";
      for (const auto& dep : targets.at("all").dependsOn) {
        os << " " << dep;
      }
      os << "\n\n";
    }

    const Vec<String> sortedTargets = topoSort(targets, targetDeps);
    for (auto itr = sortedTargets.rbegin(); itr != sortedTargets.rend();
         itr++) {
      if (*itr == ".PHONY" || *itr == "all") {
        continue;
      }

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
    if (!SOURCE_FILE_EXTS.contains(entry.path().extension())) {
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
  const String command = "cd " + OUT_DIR + " && " + CXX + " -MM " + sourceFile;
  return exec(command.c_str());
}

static void
parseMMOutput(const String& mmOutput, String& target, Vec<String>& deps) {
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
      deps.push_back(dependency);
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
      Logger::debug("contains test code: ", sourceFile);
      return true;
    }
  }
  Logger::debug("does not contain test code: ", sourceFile);
  return false;
}

static void defineDirTarget(BuildConfig& config, const String& directory) {
  config.defineTarget(directory, {"@mkdir -p $@"});
}

static void defineCompileTarget(
    BuildConfig& config, const String& objTarget, const Vec<String>& deps,
    const bool isTest = false
) {
  std::ostringstream oss;
  Logger::log(
      oss, LogLevel::status, "Compiling", deps[0].substr(6) // remove "../../"
  );

  Vec<String> commands(2);
  commands[0] = "@echo '" + oss.str() + "'";
  if (isTest) {
    commands[1] = "@$(CXX) $(CFLAGS) -DPOAC_TEST -c $< -o $@";
  } else {
    commands[1] = "@$(CXX) $(CFLAGS) -c $< -o $@";
  }
  config.defineTarget(objTarget, commands, deps);
}

static void defineLinkTarget(
    BuildConfig& config, const String& binTarget, const Vec<String>& deps
) {
  std::ostringstream oss;
  Logger::log(oss, LogLevel::status, "Linking", binTarget);

  Vec<String> commands(2);
  commands[0] = "@echo '" + oss.str() + "'";
  commands[1] = "@$(CXX) $(CFLAGS) $^ -o $@";
  config.defineTarget(binTarget, commands, deps);
}

struct ObjTargetInfo {
  String name;
  String baseDir;
  Vec<String> deps;
};

bool isDebugMode(StringRef profile) {
  if (profile == "") {
    return true;
  } else if (profile == "-d" || profile == "--debug") {
    return true;
  } else if (profile == "-r" || profile == "--release") {
    return false;
  } else {
    throw std::runtime_error(
          "invalid option: `" + String(profile) + "`\n\n"
          "       run `poac help build` for a list of options"
      );
  }
}

// Returns the directory where the Makefile is generated.
String emitMakefile(const bool debug) {
  if (!fs::exists("src")) {
    throw std::runtime_error("src directory not found");
  }
  if (!fs::exists("src/main.cc")) {
    throw std::runtime_error("src/main.cc not found");
  }
  if (!fs::exists(OUT_DIR)) {
    fs::create_directories(OUT_DIR);
  }
  if (const char* cxx = std::getenv("CXX")) {
    CXX = cxx;
  }

  const String makefilePath = OUT_DIR + "/Makefile";
  if (isMakefileUpToDate(makefilePath)) {
    Logger::debug("Makefile is up to date");
    return OUT_DIR;
  }

  const String projectName = "poac"; // TODO: Get from poac.toml
  const String pathFromOutDir = "../../";

  BuildConfig config;

  // Compiler settings
  config.defineVariable("CXX", CXX);
  const String baseCflags =
      "-Wall -Wextra -fdiagnostics-color -pedantic-errors -std=c++20 "; // TODO:
                                                                        // Get
                                                                        // from
                                                                        // poac.toml
  if (debug) {
    config.defineVariable("CFLAGS", baseCflags + "-g -O0 -DDEBUG");
  } else {
    config.defineVariable("CFLAGS", baseCflags + "-O3 -DNDEBUG");
  }

  // Build rules
  const String buildOutDir = projectName + ".d";
  defineDirTarget(config, buildOutDir);

  Vec<String> phonies = {"all"};
  config.defineTarget("all", {}, {buildOutDir, projectName});

  const Vec<String> sourceFiles = listSourceFiles("src");
  Vec<String> buildObjTargets;

  // sourceFile.cc -> ObjTargetInfo
  HashMap<String, ObjTargetInfo> objTargetInfos;
  for (const String& sourceFileName : sourceFiles) {
    const String sourceFile = pathFromOutDir + sourceFileName;
    const String mmOutput = runMM(sourceFile);

    String objTarget; // sourceFile.o
    Vec<String> objTargetDeps;
    parseMMOutput(mmOutput, objTarget, objTargetDeps);

    const String targetBaseDir =
        fs::relative(Path(sourceFile).parent_path(), pathFromOutDir + "src")
            .string();
    objTargetInfos[sourceFileName] = {objTarget, targetBaseDir, objTargetDeps};

    const String buildTargetBaseDir = buildOutDir + "/" + targetBaseDir;
    const String buildObjTarget = buildTargetBaseDir + "/" + objTarget;

    // Add a target to create the buildOutDir and buildTargetBaseDir.
    Vec<String> buildTargetDeps = objTargetDeps;
    buildTargetDeps.push_back("|"); // order-only dependency
    buildTargetDeps.push_back(buildOutDir);
    if (targetBaseDir != ".") {
      defineDirTarget(config, buildTargetBaseDir);
      buildTargetDeps.push_back(buildTargetBaseDir);
    }

    buildObjTargets.push_back(buildObjTarget);
    defineCompileTarget(config, buildObjTarget, buildTargetDeps);
  }
  defineLinkTarget(config, projectName, buildObjTargets);

  // Targets for testing.
  bool enableTesting = false;
  Vec<String> testCommands;
  Vec<String> testTargets;
  const HashSet<String> buildObjTargetSet(
      buildObjTargets.begin(), buildObjTargets.end()
  );
  const String testOutDir = "tests";
  for (auto& [sourceFile, objTargetInfo] : objTargetInfos) {
    if (containsTestCode(sourceFile)) {
      enableTesting = true;

      const String testTargetBaseDir = testOutDir + "/" + objTargetInfo.baseDir;
      const String testObjTarget =
          testTargetBaseDir + "/test_" + objTargetInfo.name;
      const String testTargetName = Path(sourceFile).stem().string();
      const String testTarget = testTargetBaseDir + "/test_" + testTargetName;

      // NOTE: Since we know that we don't use objTargetInfos for other
      // targets, we can just update it here instead of creating a copy.
      objTargetInfo.deps.push_back("|"); // order-only dependency
      objTargetInfo.deps.push_back(testOutDir);

      // Add a target to create the testTargetBaseDir.
      if (objTargetInfo.baseDir != ".") {
        defineDirTarget(config, testTargetBaseDir);
        objTargetInfo.deps.push_back(testTargetBaseDir);
      }

      // Test object target.
      defineCompileTarget(config, testObjTarget, objTargetInfo.deps, true);

      // Test binary target.
      Vec<String> testTargetDeps = {testObjTarget};
      // This test target depends on the object file corresponding to
      // the header file included in this source file.
      for (const String& header : objTargetInfo.deps) {
        // We shouldn't depend on the original object file (e.g.,
        // poac.d/path/to/file.o). We should depend on the test object
        // file (e.g., tests/path/to/test_file.o).
        if (Path(sourceFile).stem().string() == Path(header).stem().string()) {
          continue;
        }

        const Path headerPath = Path(header);
        if (!HEADER_FILE_EXTS.contains(headerPath.extension())) {
          continue;
        }

        // headerPath: src/path/to/header.h ->
        // object target: poac.d/path/to/header.o
        const String headerObjTarget =
            buildOutDir + "/"
            + (fs::relative(headerPath.parent_path(), pathFromOutDir + "src")
               / headerPath.stem())
                  .string()
            + ".o";

        auto itr = buildObjTargetSet.find(headerObjTarget);
        if (itr == buildObjTargetSet.end()) {
          continue;
        }
        testTargetDeps.push_back(*itr);
      }

      defineLinkTarget(config, testTarget, testTargetDeps);
      Logger::debug(testTarget, ':');
      for (const auto& dep : testTargetDeps) {
        Logger::debug(" '", dep, "'");
      }

      std::ostringstream oss;
      Logger::log(oss, LogLevel::status, "Testing", testTargetName);
      testCommands.push_back("@echo '" + oss.str() + "'");
      testCommands.push_back('@' + testTarget);
      testTargets.push_back(testTarget);
    }
  }
  if (enableTesting) {
    // Target to create the tests directory.
    defineDirTarget(config, testOutDir);

    config.defineTarget("test", testCommands, testTargets);
    phonies.push_back("test");
  }

  config.defineTarget(".PHONY", {}, phonies);

  std::ofstream ofs(makefilePath);
  config.emitMakefile(ofs);
  return OUT_DIR;
}

String modeString(const bool debug) {
  return debug ? "debug" : "release";
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
