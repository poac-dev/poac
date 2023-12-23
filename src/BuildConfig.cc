#include "BuildConfig.hpp"

#include "Algos.hpp"
#include "Logger.hpp"
#include "Manifest.hpp"
#include "TermColor.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctype.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>

static String OUT_DIR;
static String CXX = "clang++";
static String INCLUDES;
static String DEFINES;

static void setOutDir(const bool debug) {
  if (debug) {
    OUT_DIR = "poac-out/debug";
  } else {
    OUT_DIR = "poac-out/release";
  }
}

static String getOutDir() {
  if (OUT_DIR.empty()) {
    throw std::runtime_error("OUT_DIR is not set");
  }
  return OUT_DIR;
}

struct Target {
  Vec<String> commands;
  Vec<String> dependsOn;
};

struct BuildConfig {
  HashMap<String, String> variables;
  HashMap<String, Vec<String>> varDeps;
  HashMap<String, Target> targets;
  HashMap<String, Vec<String>> targetDeps;

  void defineVariable(String, String, const Vec<String>& = {});
  void defineTarget(String, const Vec<String>&, const Vec<String>& = {});
  void emitMakefile(std::ostream& = std::cout) const;
  void emitCompdb(StringRef, std::ostream& = std::cout) const;
};

void BuildConfig::defineVariable(
    String name, String value, const Vec<String>& dependsOn
) {
  variables[name] = value;
  for (const String& dep : dependsOn) {
    // reverse dependency
    varDeps[dep].push_back(name);
  }
}

void BuildConfig::defineTarget(
    String name, const Vec<String>& commands, const Vec<String>& dependsOn
) {
  targets[name] = {commands, dependsOn};
  for (const String& dep : dependsOn) {
    // reverse dependency
    targetDeps[dep].push_back(name);
  }
}

static void emitTarget(
    std::ostream& os, StringRef target, const Vec<String>& dependsOn,
    const Vec<String>& commands = {}
) {
  usize offset = 0;

  os << target << ":";
  offset += target.size() + 2; // : and space

  for (const String& dep : dependsOn) {
    if (offset + dep.size() + 2 > 80) { // 2 for space and \.
      // \ for line continuation. \ is the 80th character.
      os << std::setw(83 - offset) << " \\\n ";
      offset = 2;
    }
    os << " " << dep;
    offset += dep.size() + 1; // space
  }
  os << '\n';

  for (const String& cmd : commands) {
    os << '\t' << cmd << '\n';
  }
  os << '\n';
}

void BuildConfig::emitMakefile(std::ostream& os) const {
  // TODO: I guess we can topo-sort when calling defineVariable and
  // defineTarget.  Then we don't need to sort here.  This way we can
  // avoid the extra memory usage and possibly improve time complexity.
  // The current way is simple and bug-free though.

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

  if (targets.contains(".PHONY")) {
    emitTarget(os, ".PHONY", targets.at(".PHONY").dependsOn);
  }
  if (targets.contains("all")) {
    emitTarget(os, "all", targets.at("all").dependsOn);
  }
  const Vec<String> sortedTargets = topoSort(targets, targetDeps);
  for (auto itr = sortedTargets.rbegin(); itr != sortedTargets.rend(); itr++) {
    if (*itr == ".PHONY" || *itr == "all") {
      continue;
    }
    emitTarget(os, *itr, targets.at(*itr).dependsOn, targets.at(*itr).commands);
  }
}

void BuildConfig::emitCompdb(StringRef baseDir, std::ostream& os) const {
  const Path baseDirPath = fs::canonical(baseDir);
  const Vec<String> phony = targets.at(".PHONY").dependsOn;
  const HashSet<String> phonyDeps(phony.begin(), phony.end());
  const String firstIdent = String(2, ' ');
  const String secondIdent = String(4, ' ');

  std::stringstream ss;
  for (const auto& [target, targetInfo] : targets) {
    if (target == ".PHONY" || target == "all" || phonyDeps.contains(target)) {
      // Ignore .PHONY and all targets, and phony dependencies.
      continue;
    }

    bool isCompileTarget = false;
    for (StringRef cmd : targetInfo.commands) {
      if (!cmd.starts_with("$(CXX)") && !cmd.starts_with("@$(CXX)")) {
        continue;
      }
      if (cmd.find("-c") == String::npos) {
        // Ignore linking commands.
        continue;
      }
      isCompileTarget = true;
    }
    if (!isCompileTarget) {
      continue;
    }

    // The first dependency is the source file.
    const String file = targetInfo.dependsOn[0];
    // The output is the target.
    const String output = target;
    const String cmd = CXX + ' ' + variables.at("CXXFLAGS") + DEFINES + INCLUDES
                       + " -c " + file + " -o " + output;

    ss << firstIdent << "{\n";
    ss << secondIdent << "\"directory\": " << baseDirPath << ",\n";
    ss << secondIdent << "\"file\": " << std::quoted(file) << ",\n";
    ss << secondIdent << "\"output\": " << std::quoted(output) << ",\n";
    ss << secondIdent << "\"command\": " << std::quoted(cmd) << "\n";
    ss << firstIdent << "},\n";
  }

  String output = ss.str();
  if (!output.empty()) {
    // Remove the last comma.
    output.pop_back();
    output.pop_back();
  }

  os << "[\n";
  os << output << '\n';
  os << "]\n";
}

static Vec<String> listSourceFiles(StringRef directory) {
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
  const String command = "cd " + getOutDir() + " && " + CXX + DEFINES + INCLUDES
                         + " -MM " + sourceFile;
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

static bool isUpToDate(StringRef makefilePath) {
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

static String buildCmd(const String& cmd) noexcept {
  if (isVerbose()) {
    return cmd;
  } else {
    return "@" + cmd;
  }
}

static void defineDirTarget(BuildConfig& config, const String& directory) {
  config.defineTarget(directory, {buildCmd("mkdir -p $@")});
}

static void defineCompileTarget(
    BuildConfig& config, const String& objTarget, const Vec<String>& deps,
    const bool isTest = false
) {
  std::ostringstream oss;
  Logger::log(
      oss, LogLevel::info, "Compiling", deps[0].substr(6) // remove "../../"
  );

  Vec<String> commands(2);
  commands[0] = "@echo '" + oss.str() + "'";

  const String compileCmd = "$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES)";
  if (isTest) {
    commands[1] = buildCmd(compileCmd + " -DPOAC_TEST -c $< -o $@");
  } else {
    commands[1] = buildCmd(compileCmd + " -c $< -o $@");
  }
  config.defineTarget(objTarget, commands, deps);
}

static void defineLinkTarget(
    BuildConfig& config, const String& binTarget, const Vec<String>& deps
) {
  std::ostringstream oss;
  Logger::log(oss, LogLevel::info, "Linking", binTarget);

  Vec<String> commands(2);
  commands[0] = "@echo '" + oss.str() + "'";
  commands[1] = buildCmd("$(CXX) $(CXXFLAGS) $^ -o $@");
  config.defineTarget(binTarget, commands, deps);
}

struct ObjTargetInfo {
  String name;
  String baseDir;
  Vec<String> deps;
};

static BuildConfig configureBuild(const bool debug) {
  if (!fs::exists("src")) {
    throw std::runtime_error("src directory not found");
  }
  if (!fs::exists("src/main.cc")) {
    throw std::runtime_error("src/main.cc not found");
  }

  const String outDir = getOutDir();
  if (!fs::exists(outDir)) {
    fs::create_directories(outDir);
  }
  if (const char* cxx = std::getenv("CXX")) {
    CXX = cxx;
  }

  const String packageName = getPackageName();
  const String pathFromOutDir = "../../";

  BuildConfig config;

  // Variables
  config.defineVariable("CXX", CXX);
  String cxxflags =
      "-Wall -Wextra -pedantic-errors -std=c++" + getPackageEdition();
  if (shouldColor()) {
    cxxflags += " -fdiagnostics-color";
  }
  if (debug) {
    cxxflags += " -g -O0 -DDEBUG";
  } else {
    cxxflags += " -O3 -DNDEBUG";
  }
  config.defineVariable("CXXFLAGS", cxxflags);

  String packageNameUpper = packageName;
  std::transform(
      packageNameUpper.begin(), packageNameUpper.end(),
      packageNameUpper.begin(), ::toupper
  );
  DEFINES =
      " -D" + packageNameUpper + "_VERSION='\"" + getPackageVersion() + "\"'";
  config.defineVariable("DEFINES", DEFINES);

  const Vec<Path> deps = installGitDependencies();
  for (const Path& dep : deps) {
    const Path includeDir = dep / "include";
    if (fs::exists(includeDir) && fs::is_directory(includeDir)
        && !fs::is_empty(includeDir)) {
      INCLUDES += " -I" + includeDir.string();
    } else {
      INCLUDES += " -I" + dep.string();
    }
  }
  Logger::debug("INCLUDES: ", INCLUDES);
  config.defineVariable("INCLUDES", INCLUDES);

  // Build rules
  const String buildOutDir = packageName + ".d";
  defineDirTarget(config, buildOutDir);

  Vec<String> phonies = {"all"};
  config.defineTarget("all", {}, {packageName});

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

    // Add a target to create the buildOutDir and buildTargetBaseDir.
    Vec<String> buildTargetDeps = objTargetDeps;
    buildTargetDeps.emplace_back("|"); // order-only dependency
    buildTargetDeps.push_back(buildOutDir);
    String buildTargetBaseDir = buildOutDir;
    if (targetBaseDir != ".") {
      buildTargetBaseDir += "/" + targetBaseDir;
      defineDirTarget(config, buildTargetBaseDir);
      buildTargetDeps.push_back(buildTargetBaseDir);
    }

    const String buildObjTarget = buildTargetBaseDir + "/" + objTarget;
    buildObjTargets.push_back(buildObjTarget);
    defineCompileTarget(config, buildObjTarget, buildTargetDeps);
  }
  defineLinkTarget(config, packageName, buildObjTargets);

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

      // NOTE: Since we know that we don't use objTargetInfos for other
      // targets, we can just update it here instead of creating a copy.
      objTargetInfo.deps.emplace_back("|"); // order-only dependency
      objTargetInfo.deps.push_back(testOutDir);

      // Add a target to create the testTargetBaseDir.
      String testTargetBaseDir = testOutDir;
      if (objTargetInfo.baseDir != ".") {
        testTargetBaseDir += "/" + objTargetInfo.baseDir;
        defineDirTarget(config, testTargetBaseDir);
        objTargetInfo.deps.push_back(testTargetBaseDir);
      }

      const String testObjTarget =
          testTargetBaseDir + "/test_" + objTargetInfo.name;
      const String testTargetName = Path(sourceFile).stem().string();
      const String testTarget = testTargetBaseDir + "/test_" + testTargetName;

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
        const Path headerPath(header);
        if (Path(sourceFile).stem().string() == headerPath.stem().string()) {
          continue;
        }
        if (!HEADER_FILE_EXTS.contains(headerPath.extension())) {
          continue;
        }

        // headerPath: src/path/to/header.h ->
        // object target: poac.d/path/to/header.o
        String headerObjTargetBaseDir =
            fs::relative(headerPath.parent_path(), pathFromOutDir + "src")
                .string();
        if (headerObjTargetBaseDir != ".") {
          headerObjTargetBaseDir = buildOutDir + "/" + headerObjTargetBaseDir;
        } else {
          headerObjTargetBaseDir = buildOutDir;
        }
        const String headerObjTarget =
            headerObjTargetBaseDir + "/" + headerPath.stem().string() + ".o";
        Logger::debug("headerObjTarget: ", headerObjTarget);

        auto itr = buildObjTargetSet.find(headerObjTarget);
        if (itr == buildObjTargetSet.end()) {
          continue;
        }
        testTargetDeps.push_back(*itr);
      }

      defineLinkTarget(config, testTarget, testTargetDeps);
      Logger::debug(testTarget, ':');
      for (const String& dep : testTargetDeps) {
        Logger::debug(" '", dep, "'");
      }

      std::ostringstream oss;
      Logger::log(oss, LogLevel::info, "Testing", testTargetName);
      testCommands.push_back("@echo '" + oss.str() + "'");
      testCommands.push_back(buildCmd(testTarget));
      testTargets.push_back(testTarget);
    }
  }
  if (enableTesting) {
    // Target to create the tests directory.
    defineDirTarget(config, testOutDir);
    config.defineTarget("test", testCommands, testTargets);
    phonies.emplace_back("test");
  }

  config.defineTarget(".PHONY", {}, phonies);

  return config;
}

/// @returns the directory where the Makefile is generated.
String emitMakefile(const bool debug) {
  setOutDir(debug);

  const String outDir = getOutDir();
  const String makefilePath = outDir + "/Makefile";
  if (isUpToDate(makefilePath)) {
    Logger::debug("Makefile is up to date");
    return outDir;
  }

  const BuildConfig config = configureBuild(debug);
  std::ofstream ofs(makefilePath);
  config.emitMakefile(ofs);
  return outDir;
}

/// @returns the directory where the compilation database is generated.
String emitCompdb(const bool debug) {
  setOutDir(debug);

  const String outDir = getOutDir();
  const String compdbPath = outDir + "/compile_commands.json";
  if (isUpToDate(compdbPath)) {
    Logger::debug("compile_commands.json is up to date");
    return outDir;
  }

  const BuildConfig config = configureBuild(debug);
  std::ofstream ofs(compdbPath);
  config.emitCompdb(outDir, ofs);
  return outDir;
}

String modeString(const bool debug) {
  return debug ? "debug" : "release";
}

String getMakeCommand() {
  if (isVerbose()) {
    return "make";
  } else {
    return "make -s --no-print-directory";
  }
}

#ifdef POAC_TEST

#  include <cassert>

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
