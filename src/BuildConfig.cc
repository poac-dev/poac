#include "BuildConfig.hpp"

#include "Algos.hpp"
#include "Exception.hpp"
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
#include <span>
#include <sstream>
#include <stdio.h>
#include <string>
#include <thread>

static String OUT_DIR;
static constexpr StringRef TEST_OUT_DIR = "tests";
static const String PATH_FROM_OUT_DIR = "../..";
static String CXX = "clang++";
static String CXXFLAGS = " -Wall -Wextra -pedantic-errors -std=c++";
static String DEFINES;
static String INCLUDES = " -Iinclude";
static String LIBS;

void setOutDir(const bool isDebug) {
  if (isDebug) {
    OUT_DIR = "poac-out/debug";
  } else {
    OUT_DIR = "poac-out/release";
  }
}
String getOutDir() {
  if (OUT_DIR.empty()) {
    throw PoacError("outDir is not set");
  }
  return OUT_DIR;
}

static Vec<Path> listSourceFilePaths(StringRef directory) {
  Vec<Path> sourceFilePaths;
  for (const auto& entry : fs::recursive_directory_iterator(directory)) {
    if (!SOURCE_FILE_EXTS.contains(entry.path().extension())) {
      continue;
    }
    sourceFilePaths.emplace_back(entry.path());
  }
  return sourceFilePaths;
}

enum class VarType {
  Recursive, // =
  Simple, // :=
  Cond, // ?=
  Append, // +=
  Shell, // !=
};

std::ostream& operator<<(std::ostream& os, VarType type) {
  switch (type) {
    case VarType::Recursive:
      os << "=";
      break;
    case VarType::Simple:
      os << ":=";
      break;
    case VarType::Cond:
      os << "?=";
      break;
    case VarType::Append:
      os << "+=";
      break;
    case VarType::Shell:
      os << "!=";
      break;
  }
  return os;
}

struct Variable {
  String value;
  VarType type;
};

std::ostream& operator<<(std::ostream& os, const Variable& var) {
  os << var.type << ' ' << var.value;
  return os;
}

struct Target {
  Vec<String> commands;
  OrderedHashSet<String> dependsOn;
};

struct BuildConfig {
  const String packageName;
  const Path buildOutDir;

  HashMap<String, Variable> variables;
  HashMap<String, Vec<String>> varDeps;
  HashMap<String, Target> targets;
  HashMap<String, Vec<String>> targetDeps;
  Option<Target> phony;
  Option<Target> all;

  BuildConfig() = default;
  explicit BuildConfig(const String& packageName)
      : packageName(packageName), buildOutDir(packageName + ".d") {}

  void defineVariable(
      const String&, const Variable&, const OrderedHashSet<String>& = {}
  );
  void defineSimpleVariable(
      const String&, const String&, const OrderedHashSet<String>& = {}
  );
  void defineCondVariable(
      const String&, const String&, const OrderedHashSet<String>& = {}
  );

  void
  defineTarget(String, const Vec<String>&, const OrderedHashSet<String>& = {});
  void setPhony(const OrderedHashSet<String>&);
  void setAll(const OrderedHashSet<String>&);
  void emitMakefile(std::ostream& = std::cout) const;
  void emitCompdb(StringRef, std::ostream& = std::cout) const;
};

void BuildConfig::defineVariable(
    const String& name, const Variable& value,
    const OrderedHashSet<String>& dependsOn
) {
  variables[name] = value;
  for (const String& dep : dependsOn) {
    // reverse dependency
    varDeps[dep].push_back(name);
  }
}
void BuildConfig::defineSimpleVariable(
    const String& name, const String& value,
    const OrderedHashSet<String>& dependsOn
) {
  defineVariable(name, { value, VarType::Simple }, dependsOn);
}
void BuildConfig::defineCondVariable(
    const String& name, const String& value,
    const OrderedHashSet<String>& dependsOn
) {
  defineVariable(name, { value, VarType::Cond }, dependsOn);
}

void BuildConfig::defineTarget(
    String name, const Vec<String>& commands,
    const OrderedHashSet<String>& dependsOn
) {
  targets[name] = { commands, dependsOn };
  for (const String& dep : dependsOn) {
    // reverse dependency
    targetDeps[dep].push_back(name);
  }
}

void BuildConfig::setPhony(const OrderedHashSet<String>& dependsOn) {
  phony = { {}, dependsOn };
}
void BuildConfig::setAll(const OrderedHashSet<String>& dependsOn) {
  all = { {}, dependsOn };
}

static void emitTarget(
    std::ostream& os, StringRef target, std::span<const String> dependsOn,
    std::span<const String> commands = {}
) {
  usize offset = 0;

  os << target << ":";
  offset += target.size() + 2; // : and space

  for (StringRef dep : dependsOn) {
    if (offset + dep.size() + 2 > 80) { // 2 for space and \.
      // \ for line continuation. \ is the 80th character.
      os << std::setw(83 - offset) << " \\\n ";
      offset = 2;
    }
    os << " " << dep;
    offset += dep.size() + 1; // space
  }
  os << '\n';

  for (StringRef cmd : commands) {
    os << '\t' << cmd << '\n';
  }
  os << '\n';
}

void BuildConfig::emitMakefile(std::ostream& os) const {
  const Vec<String> sortedVars = topoSort(variables, varDeps);
  for (const String& varName : sortedVars) {
    os << varName << ' ' << variables.at(varName) << '\n';
  }
  if (!sortedVars.empty() && !targets.empty()) {
    os << '\n';
  }

  if (phony.has_value()) {
    emitTarget(os, ".PHONY", phony->dependsOn);
  }
  if (all.has_value()) {
    emitTarget(os, "all", all->dependsOn);
  }
  const Vec<String> sortedTargets = topoSort(targets, targetDeps);
  for (auto itr = sortedTargets.rbegin(); itr != sortedTargets.rend(); itr++) {
    emitTarget(os, *itr, targets.at(*itr).dependsOn, targets.at(*itr).commands);
  }
}

void BuildConfig::emitCompdb(StringRef baseDir, std::ostream& os) const {
  const Path baseDirPath = fs::canonical(baseDir);
  const String firstIdent(2, ' ');
  const String secondIdent(4, ' ');

  std::stringstream ss;
  for (const auto& [target, targetInfo] : targets) {
    if (phony->dependsOn.contains(target)) {
      // Ignore phony dependencies.
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
    const String cmd = CXX + ' ' + CXXFLAGS + DEFINES + INCLUDES + " -c " + file
                       + " -o " + output;

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
    output.pop_back(); // \n
    output.pop_back(); // ,
  }

  os << "[\n";
  os << output << '\n';
  os << "]\n";
}

String runMM(const String& sourceFile, const bool isTest = false) {
  String command = "cd " + getOutDir() + " && " + CXX + DEFINES + INCLUDES;
  if (isTest) {
    command += " -DPOAC_TEST -MM " + sourceFile;
  } else {
    command += " -MM " + sourceFile;
  }
  return getCmdOutput(command);
}

static OrderedHashSet<String>
parseMMOutput(const String& mmOutput, String& target) {
  std::istringstream iss(mmOutput);
  std::getline(iss, target, ':');
  Logger::debug(target, ':');

  String dependency;
  OrderedHashSet<String> deps;
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
  return deps;
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

static void defineDirTarget(BuildConfig& config, const Path& directory) {
  config.defineTarget(directory, { buildCmd("mkdir -p $@") });
}

static String echoCmd(StringRef header, StringRef body) {
  std::ostringstream oss;
  Logger::log(oss, LogLevel::info, header, body);
  return "@echo '" + oss.str() + "'";
}

static void defineCompileTarget(
    BuildConfig& config, const String& objTarget,
    const OrderedHashSet<String>& deps, const bool isTest = false
) {
  Vec<String> commands(2);
  commands[0] = echoCmd("Compiling", deps[0].substr(6)); // remove "../../"

  const String compileCmd = "$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES)";
  if (isTest) {
    commands[1] = buildCmd(compileCmd + " -DPOAC_TEST -c $< -o $@");
  } else {
    commands[1] = buildCmd(compileCmd + " -c $< -o $@");
  }
  config.defineTarget(objTarget, commands, deps);
}

static void defineLinkTarget(
    BuildConfig& config, const String& binTarget,
    const OrderedHashSet<String>& deps
) {
  Vec<String> commands(2);
  commands[0] = echoCmd("Linking", binTarget);
  commands[1] = buildCmd("$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@");
  config.defineTarget(binTarget, commands, deps);
}

static void collectBinDepObjs(
    OrderedHashSet<String>& deps, const OrderedHashSet<String>& objTargetDeps,
    const Path& sourceFile, const OrderedHashSet<String>& buildObjTargets,
    const BuildConfig& config
) {
  // This test target depends on the object file corresponding to
  // the header file included in this source file.
  for (const Path headerPath : objTargetDeps) {
    if (sourceFile.stem() == headerPath.stem()) {
      // We shouldn't depend on the original object file (e.g.,
      // poac.d/path/to/file.o). We should depend on the test object
      // file (e.g., tests/path/to/test_file.o).
      continue;
    }
    if (!HEADER_FILE_EXTS.contains(headerPath.extension())) {
      continue;
    }

    // Map headerPath to the corresponding object target.
    // headerPath: src/path/to/header.h ->
    // object target: poac.d/path/to/header.o
    Path headerObjTargetBaseDir =
        fs::relative(headerPath.parent_path(), PATH_FROM_OUT_DIR / "src"_path);
    if (headerObjTargetBaseDir != ".") {
      headerObjTargetBaseDir = config.buildOutDir / headerObjTargetBaseDir;
    } else {
      headerObjTargetBaseDir = config.buildOutDir;
    }
    const String headerObjTarget =
        (headerObjTargetBaseDir / headerPath.stem()).string() + ".o";
    Logger::debug("headerObjTarget: ", headerObjTarget);

    if (deps.contains(headerObjTarget)) {
      continue;
    }

    if (!buildObjTargets.contains(headerObjTarget)) {
      // If the header file is not included in the source file, we
      // should not depend on the object file corresponding to the
      // header file.
      continue;
    }
    deps.push_back(headerObjTarget);
    Logger::debug("headerObjTarget: added ", headerObjTarget);
    collectBinDepObjs(
        deps, config.targets.at(headerObjTarget).dependsOn, sourceFile,
        buildObjTargets, config
    );
  }
}

static void setVariables(BuildConfig& config, const bool isDebug) {
  config.defineCondVariable("CXX", CXX);

  CXXFLAGS += getPackageEdition();
  if (shouldColor()) {
    CXXFLAGS += " -fdiagnostics-color";
  }
  if (isDebug) {
    CXXFLAGS += " -g -O0 -DDEBUG";
  } else {
    CXXFLAGS += " -O3 -DNDEBUG";
  }
  const Profile profile = isDebug ? getDebugProfile() : getReleaseProfile();
  if (profile.lto) {
    CXXFLAGS += " -flto";
  }
  config.defineSimpleVariable("CXXFLAGS", CXXFLAGS);

  String packageNameUpper = config.packageName;
  std::transform(
      packageNameUpper.begin(), packageNameUpper.end(),
      packageNameUpper.begin(), ::toupper
  );
  DEFINES = " -D" + packageNameUpper + "_VERSION='\""
            + getPackageVersion().to_string() + "\"'";
  config.defineSimpleVariable("DEFINES", DEFINES);

  const Vec<DepMetadata> deps = installDependencies();
  for (const DepMetadata& dep : deps) {
    INCLUDES += ' ' + dep.includes;
    LIBS += ' ' + dep.libs;
  }
  Logger::debug("INCLUDES: ", INCLUDES);
  Logger::debug("LIBS: ", LIBS);
  config.defineSimpleVariable("INCLUDES", INCLUDES);
  config.defineSimpleVariable("LIBS", LIBS);
}

static BuildConfig configureBuild(const bool isDebug) {
  if (!fs::exists("src")) {
    throw PoacError("src directory not found");
  }
  if (!fs::exists("src/main.cc")) {
    // For now, we only support .cc extension only for the main file.
    throw PoacError("src/main.cc not found");
  }

  const String outDir = getOutDir();
  if (!fs::exists(outDir)) {
    fs::create_directories(outDir);
  }
  if (const char* cxx = std::getenv("CXX")) {
    CXX = cxx;
  }

  BuildConfig config(getPackageName());
  setVariables(config, isDebug);

  // Build rules
  defineDirTarget(config, config.buildOutDir);
  config.setAll({ config.packageName });

  const Vec<Path> sourceFilePaths = listSourceFilePaths("src");
  OrderedHashSet<String> buildObjTargets;
  for (const Path& sourceFilePath : sourceFilePaths) {
    const Path sourceFileRelPath = PATH_FROM_OUT_DIR / sourceFilePath;
    String objTarget; // source.o
    OrderedHashSet<String> objTargetDeps =
        parseMMOutput(runMM(sourceFileRelPath), objTarget);

    const Path targetBaseDir = fs::relative(
        sourceFileRelPath.parent_path(), PATH_FROM_OUT_DIR / "src"_path
    );

    // Add a target to create the buildOutDir and buildTargetBaseDir.
    objTargetDeps.push_back("|"); // order-only dependency
    objTargetDeps.push_back(config.buildOutDir);
    Path buildTargetBaseDir = config.buildOutDir;
    if (targetBaseDir != ".") {
      buildTargetBaseDir /= targetBaseDir;
      defineDirTarget(config, buildTargetBaseDir);
      objTargetDeps.push_back(buildTargetBaseDir);
    }

    const String buildObjTarget = buildTargetBaseDir / objTarget;
    buildObjTargets.push_back(buildObjTarget);
    defineCompileTarget(config, buildObjTarget, objTargetDeps);
  }
  // Project binary target.
  const String mainObjTarget = config.buildOutDir / "main.o";
  OrderedHashSet<String> projTargetDeps = { mainObjTarget };
  collectBinDepObjs(
      projTargetDeps, config.targets.at(mainObjTarget).dependsOn, "",
      buildObjTargets, config
  );
  defineLinkTarget(config, config.packageName, projTargetDeps);

  // Targets for tests.
  bool enableTesting = false;
  Vec<String> testCommands;
  OrderedHashSet<String> testTargets;
  for (const Path& sourceFilePath : sourceFilePaths) {
    if (!containsTestCode(sourceFilePath)) {
      continue;
    }
    enableTesting = true;

    const Path sourceFileRelPath = PATH_FROM_OUT_DIR / Path(sourceFilePath);
    String objTarget; // source.o
    OrderedHashSet<String> objTargetDeps =
        parseMMOutput(runMM(sourceFileRelPath, true /* isTest */), objTarget);

    const Path targetBaseDir = fs::relative(
        sourceFileRelPath.parent_path(), PATH_FROM_OUT_DIR / "src"_path
    );

    // Add a target to create the testTargetBaseDir.
    objTargetDeps.push_back("|"); // order-only dependency
    objTargetDeps.push_back(String(TEST_OUT_DIR));
    Path testTargetBaseDir = TEST_OUT_DIR;
    if (targetBaseDir != ".") {
      testTargetBaseDir /= targetBaseDir;
      defineDirTarget(config, testTargetBaseDir);
      objTargetDeps.push_back(testTargetBaseDir);
    }

    const String testObjTarget =
        (testTargetBaseDir / "test_").string() + objTarget;
    const String testTargetName = sourceFileRelPath.stem().string();
    const String testTarget =
        (testTargetBaseDir / "test_").string() + testTargetName;
    Logger::debug("testTarget: ", testTarget);

    // Test object target.
    defineCompileTarget(
        config, testObjTarget, objTargetDeps, true /* isTest */
    );

    // Test binary target.
    OrderedHashSet<String> testTargetDeps = { testObjTarget };
    collectBinDepObjs(
        testTargetDeps, objTargetDeps, sourceFileRelPath, buildObjTargets,
        config
    );
    defineLinkTarget(config, testTarget, testTargetDeps);
    Logger::debug(testTarget, ':');
    for (StringRef dep : testTargetDeps) {
      Logger::debug(" '", dep, "'");
    }

    testCommands.emplace_back(echoCmd("Testing", testTargetName));
    testCommands.emplace_back(buildCmd(testTarget));
    testTargets.push_back(testTarget);
  }

  OrderedHashSet<String> phonies = { "all" };
  if (enableTesting) {
    // Target to create the tests directory.
    defineDirTarget(config, TEST_OUT_DIR);
    config.defineTarget("test", testCommands, testTargets);
    phonies.push_back("test");
  }
  config.setPhony(phonies);

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

String getMakeCommand(const bool isParallel) {
  String makeCommand;
  if (isVerbose()) {
    makeCommand = "make";
  } else {
    makeCommand = "make -s --no-print-directory";
  }

  if (isParallel) {
    const unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads > 1) {
      makeCommand += " -j" + std::to_string(numThreads);
    }
  }

  return makeCommand;
}

#ifdef POAC_TEST

#  include "TestUtils.hpp"

void test_cycle_vars() {
  BuildConfig config;
  config.defineSimpleVariable("a", "b", { "b" });
  config.defineSimpleVariable("b", "c", { "c" });
  config.defineSimpleVariable("c", "a", { "a" });

  ASSERT_EXCEPTION(std::stringstream ss; config.emitMakefile(ss), PoacError,
                                         "too complex build graph");
}

void test_simple_vars() {
  BuildConfig config;
  config.defineSimpleVariable("c", "3", { "b" });
  config.defineSimpleVariable("b", "2", { "a" });
  config.defineSimpleVariable("a", "1");

  std::stringstream ss;
  config.emitMakefile(ss);

  ASSERT_EQ(
      ss.str(),
      "a := 1\n"
      "b := 2\n"
      "c := 3\n"
  );
}

void test_depend_on_unregistered_var() {
  BuildConfig config;
  config.defineSimpleVariable("a", "1", { "b" });

  std::stringstream ss;
  config.emitMakefile(ss);

  ASSERT_EQ(ss.str(), "a := 1\n");
}

void test_cycle_targets() {
  BuildConfig config;
  config.defineTarget("a", { "echo a" }, { "b" });
  config.defineTarget("b", { "echo b" }, { "c" });
  config.defineTarget("c", { "echo c" }, { "a" });

  ASSERT_EXCEPTION(std::stringstream ss; config.emitMakefile(ss), PoacError,
                                         "too complex build graph");
}

void test_simple_targets() {
  BuildConfig config;
  config.defineTarget("a", { "echo a" });
  config.defineTarget("b", { "echo b" }, { "a" });
  config.defineTarget("c", { "echo c" }, { "b" });

  std::stringstream ss;
  config.emitMakefile(ss);

  ASSERT_EQ(
      ss.str(),
      "c: b\n"
      "\techo c\n"
      "\n"
      "b: a\n"
      "\techo b\n"
      "\n"
      "a:\n"
      "\techo a\n"
      "\n"
  );
}

void test_depend_on_unregistered_target() {
  BuildConfig config;
  config.defineTarget("a", { "echo a" }, { "b" });

  std::stringstream ss;
  config.emitMakefile(ss);

  ASSERT_EQ(
      ss.str(),
      "a: b\n"
      "\techo a\n"
      "\n"
  );
}

int main() {
  REGISTER_TEST(test_cycle_vars);
  REGISTER_TEST(test_simple_vars);
  REGISTER_TEST(test_depend_on_unregistered_var);
  REGISTER_TEST(test_cycle_targets);
  REGISTER_TEST(test_simple_targets);
  REGISTER_TEST(test_depend_on_unregistered_target);
}
#endif
