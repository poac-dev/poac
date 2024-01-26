#include "BuildConfig.hpp"

#include "Algos.hpp"
#include "Exception.hpp"
#include "Git2.hpp"
#include "Logger.hpp"
#include "Manifest.hpp"
#include "TermColor.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ostream>
#include <span>
#include <sstream>
#include <string>
#include <thread>

static constinit const StringRef TEST_OUT_DIR = "tests";
static constinit const StringRef PATH_FROM_OUT_DIR = "../../";

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
static String OUT_DIR;
static String CXX = "clang++";
static String CXXFLAGS;
static String DEFINES;
static String INCLUDES = " -Iinclude";
static String LIBS;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

void
setOutDir(const bool isDebug) {
  if (isDebug) {
    OUT_DIR = "poac-out/debug";
  } else {
    OUT_DIR = "poac-out/release";
  }
}
String
getOutDir() {
  if (OUT_DIR.empty()) {
    throw PoacError("outDir is not set");
  }
  return OUT_DIR;
}

static Vec<Path>
listSourceFilePaths(const StringRef directory) {
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

std::ostream&
operator<<(std::ostream& os, VarType type) {
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
  VarType type = VarType::Simple;
};

std::ostream&
operator<<(std::ostream& os, const Variable& var) {
  os << var.type << ' ' << var.value;
  return os;
}

struct Target {
  Vec<String> commands;
  Option<String> sourceFile;
  HashSet<String> remDeps;
};

struct BuildConfig {
  String packageName;
  Path buildOutDir;

  HashMap<String, Variable> variables;
  HashMap<String, Vec<String>> varDeps;
  HashMap<String, Target> targets;
  HashMap<String, Vec<String>> targetDeps;
  Option<HashSet<String>> phony;
  Option<HashSet<String>> all;

  BuildConfig() = default;
  explicit BuildConfig(const String& packageName)
      : packageName(packageName), buildOutDir(packageName + ".d") {}

  void defineVar(
      const String& name, const Variable& value,
      const HashSet<String>& dependsOn = {}
  ) {
    variables[name] = value;
    for (const String& dep : dependsOn) {
      // reverse dependency
      varDeps[dep].push_back(name);
    }
  }
  void defineSimpleVar(
      const String& name, const String& value,
      const HashSet<String>& dependsOn = {}
  ) {
    defineVar(name, { value, VarType::Simple }, dependsOn);
  }
  void defineCondVar(
      const String& name, const String& value,
      const HashSet<String>& dependsOn = {}
  ) {
    defineVar(name, { value, VarType::Cond }, dependsOn);
  }

  void defineTarget(
      const String& name, const Vec<String>& commands,
      const HashSet<String>& remDeps = {},
      const Option<String>& sourceFile = None
  ) {
    targets[name] = { commands, sourceFile, remDeps };

    if (sourceFile.has_value()) {
      targetDeps[sourceFile.value()].push_back(name);
    }
    for (const String& dep : remDeps) {
      // reverse dependency
      targetDeps[dep].push_back(name);
    }
  }

  void addPhony(const String& target) {
    if (!phony.has_value()) {
      phony = { target };
    } else {
      phony->insert(target);
    }
  }

  void setAll(const HashSet<String>& dependsOn) {
    all = dependsOn;
  }

  void emitMakefile(std::ostream& os) const;
  void emitCompdb(StringRef baseDir, std::ostream& os) const;
};

static void
emitDep(std::ostream& os, usize& offset, const StringRef dep) {
  constexpr usize MAX_LINE_LEN = 80;
  if (offset + dep.size() + 2 > MAX_LINE_LEN) { // 2 for space and \.
    // \ for line continuation. \ is the 80th character.
    os << std::setw(static_cast<int>(MAX_LINE_LEN + 3 - offset)) << " \\\n ";
    offset = 2;
  }
  os << " " << dep;
  offset += dep.size() + 1; // space
}

static void
emitTarget(
    std::ostream& os, const StringRef target, const HashSet<String>& dependsOn,
    const Option<String>& sourceFile = None, const Vec<String>& commands = {}
) {
  usize offset = 0;

  os << target << ":";
  offset += target.size() + 2; // : and space

  if (sourceFile.has_value()) {
    emitDep(os, offset, sourceFile.value());
  }
  for (const StringRef dep : dependsOn) {
    emitDep(os, offset, dep);
  }
  os << '\n';

  for (const StringRef cmd : commands) {
    os << '\t';
    if (!cmd.starts_with('@')) {
      os << "$(Q)";
    }
    os << cmd << '\n';
  }
  os << '\n';
}

void
BuildConfig::emitMakefile(std::ostream& os) const {
  const Vec<String> sortedVars = topoSort(variables, varDeps);
  for (const String& varName : sortedVars) {
    os << varName << ' ' << variables.at(varName) << '\n';
  }
  if (!sortedVars.empty() && !targets.empty()) {
    os << '\n';
  }

  if (phony.has_value()) {
    emitTarget(os, ".PHONY", phony.value());
  }
  if (all.has_value()) {
    emitTarget(os, "all", all.value());
  }
  const Vec<String> sortedTargets = topoSort(targets, targetDeps);
  // NOLINTNEXTLINE(modernize-loop-convert)
  for (auto itr = sortedTargets.rbegin(); itr != sortedTargets.rend(); itr++) {
    emitTarget(
        os, *itr, targets.at(*itr).remDeps, targets.at(*itr).sourceFile,
        targets.at(*itr).commands
    );
  }
}

void
BuildConfig::emitCompdb(const StringRef baseDir, std::ostream& os) const {
  const Path baseDirPath = fs::canonical(baseDir);
  const String indent1(2, ' ');
  const String indent2(4, ' ');

  std::ostringstream oss;
  for (const auto& [target, targetInfo] : targets) {
    if (phony->contains(target)) {
      // Ignore phony dependencies.
      continue;
    }

    bool isCompileTarget = false;
    for (const StringRef cmd : targetInfo.commands) {
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

    // We don't check the Option value because we know the first dependency
    // always exists for compile targets.
    const String file = targetInfo.sourceFile.value();
    // The output is the target.
    const String output = target;
    String cmd = CXX;
    cmd += ' ';
    cmd += CXXFLAGS;
    cmd += DEFINES;
    cmd += INCLUDES;
    cmd += " -c ";
    cmd += file;
    cmd += " -o ";
    cmd += output;

    oss << indent1 << "{\n";
    oss << indent2 << "\"directory\": " << baseDirPath << ",\n";
    oss << indent2 << "\"file\": " << std::quoted(file) << ",\n";
    oss << indent2 << "\"output\": " << std::quoted(output) << ",\n";
    oss << indent2 << "\"command\": " << std::quoted(cmd) << "\n";
    oss << indent1 << "},\n";
  }

  String output = oss.str();
  if (!output.empty()) {
    // Remove the last comma.
    output.pop_back(); // \n
    output.pop_back(); // ,
  }

  os << "[\n";
  os << output << '\n';
  os << "]\n";
}

String
runMM(const String& sourceFile, const bool isTest = false) {
  String command =
      "cd " + getOutDir() + " && " + CXX + CXXFLAGS + DEFINES + INCLUDES;
  if (isTest) {
    command += " -DPOAC_TEST";
  }
  command += " -MM " + sourceFile;
  return getCmdOutput(command);
}

static HashSet<String>
parseMMOutput(const String& mmOutput, String& target) {
  std::istringstream iss(mmOutput);
  std::getline(iss, target, ':');

  String dependency;
  HashSet<String> deps;
  bool isFirst = true;
  while (std::getline(iss, dependency, ' ')) {
    if (!dependency.empty() && dependency.front() != '\\') {
      // Remove trailing newline if it exists
      if (dependency.back() == '\n') {
        dependency.pop_back();
      }
      // Drop the first dependency because it is the source file itself,
      // which we already know.
      if (isFirst) {
        isFirst = false;
        continue;
      }
      deps.insert(dependency);
    }
  }
  return deps;
}

static bool
isUpToDate(const StringRef makefilePath) {
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
  return fs::last_write_time("poac.toml") <= makefileTime;
}

static bool
containsTestCode(const String& sourceFile) {
  std::ifstream ifs(sourceFile);
  String line;
  while (std::getline(ifs, line)) {
    if (line.find("POAC_TEST") != String::npos) {
      // TODO: Can't we somehow elegantly make the compiler command sharable?
      String command = CXX;
      command += " -E ";
      command += CXXFLAGS;
      command += DEFINES;
      command += INCLUDES;
      command += " " + sourceFile;

      const String src = getCmdOutput(command);

      command += " -DPOAC_TEST";
      const String testSrc = getCmdOutput(command);

      // If the source file contains POAC_TEST, by processing the source
      // file with -E, we can check if the source file contains POAC_TEST
      // or not semantically.  If the source file contains POAC_TEST, the
      // test source file should be different from the original source
      // file.
      const bool containsTest = src != testSrc;
      if (containsTest) {
        Logger::debug("Found test code: ", sourceFile);
      }
      return containsTest;
    }
  }
  return false;
}

static String
printfCmd(const StringRef header, const StringRef body) {
  std::ostringstream oss;
  Logger::info(oss, header, body);
  String msg = oss.str();

  // Replace all occurrences of '\n' with "\\n" to escape newlines
  size_t pos = 0;
  while ((pos = msg.find('\n', pos)) != String::npos) {
    msg.replace(pos, 1, "\\n");
    pos += 2; // Move past the replacement
  }

  return "@printf '" + msg + "' >&2";
}

static void
defineCompileTarget(
    BuildConfig& config, const String& objTarget, const String& sourceFile,
    const HashSet<String>& remDeps, const bool isTest = false
) {
  Vec<String> commands(3);
  commands[0] = "@mkdir -p $(@D)";
  commands[1] =
      printfCmd("Compiling", sourceFile.substr(PATH_FROM_OUT_DIR.size()));
  commands[2] = "$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES)";
  if (isTest) {
    commands[2] += " -DPOAC_TEST";
  }
  commands[2] += " -c $< -o $@";
  config.defineTarget(objTarget, commands, remDeps, sourceFile);
}

static void
defineLinkTarget(
    BuildConfig& config, const String& binTarget, const HashSet<String>& deps
) {
  Vec<String> commands(2);
  commands[0] = printfCmd("Linking", binTarget);
  commands[1] = "$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@";
  config.defineTarget(binTarget, commands, deps);
}

// Map a path to header file to the corresponding object file.
//
// e.g., src/path/to/header.h -> poac.d/path/to/header.o
static String
mapHeaderToObj(const Path& headerPath, const Path& buildOutDir) {
  Path objBaseDir =
      fs::relative(headerPath.parent_path(), PATH_FROM_OUT_DIR / "src"_path);
  if (objBaseDir != ".") {
    objBaseDir = buildOutDir / objBaseDir;
  } else {
    objBaseDir = buildOutDir;
  }
  return (objBaseDir / headerPath.stem()).string() + ".o";
}

// Recursively collect depending object files for a binary target.
// We know the binary depends on some header files.  We need to find
// if there is the corresponding object file for the header file.
// If it is, we should depend on the object file and recursively
// collect depending object files of the object file.
//
// Header files are known via -MM outputs.  Each -MM output is run
// for each source file.  So, we need objTargetDeps, which is the
// depending header files for the source file.
static void
collectBinDepObjs(
    HashSet<String>& deps, const String& sourceFileName,
    const HashSet<String>& objTargetDeps,
    const HashSet<String>& buildObjTargets, const BuildConfig& config
) {
  for (const Path headerPath : objTargetDeps) {
    if (sourceFileName == headerPath.stem()) {
      // We shouldn't depend on the original object file (e.g.,
      // poac.d/path/to/file.o). We should depend on the test object
      // file (e.g., tests/path/to/test_file.o).
      continue;
    }
    if (!HEADER_FILE_EXTS.contains(headerPath.extension())) {
      // We only care about header files.
      continue;
    }

    const String objTarget = mapHeaderToObj(headerPath, config.buildOutDir);
    if (deps.contains(objTarget)) {
      // We already added this object file.
      continue;
    }
    if (!buildObjTargets.contains(objTarget)) {
      // If the header file is not included in the source file, we
      // should not depend on the object file corresponding to the
      // header file.
      continue;
    }

    deps.insert(objTarget);
    collectBinDepObjs(
        deps, sourceFileName,
        config.targets.at(objTarget).remDeps, // we don't need sourceFile
        buildObjTargets, config
    );
  }
}

// TODO: Naming is not good, but using different namespaces for installDeps
// and installDependencies is not good either.
void
installDeps() {
  const Vec<DepMetadata> deps = installDependencies();
  for (const DepMetadata& dep : deps) {
    INCLUDES += ' ' + dep.includes;
    LIBS += ' ' + dep.libs;
  }
  Logger::debug("INCLUDES: ", INCLUDES);
  Logger::debug("LIBS: ", LIBS);
}

static void
addDefine(const String& name, const String& value) {
  DEFINES += " -D" + name + "='\"" + value + "\"'";
}

static void
setVariables(BuildConfig& config, const bool isDebug) {
  config.defineCondVar("CXX", CXX);

  CXXFLAGS += " -std=c++" + getPackageEdition().getString();
  if (shouldColor()) {
    CXXFLAGS += " -fdiagnostics-color";
  }
  if (isDebug) {
    CXXFLAGS += " -g -O0 -DDEBUG";
  } else {
    CXXFLAGS += " -O3 -DNDEBUG";
  }
  const Profile& profile = isDebug ? getDebugProfile() : getReleaseProfile();
  if (profile.lto) {
    CXXFLAGS += " -flto";
  }
  for (const String& flag : profile.cxxflags) {
    CXXFLAGS += ' ' + flag;
  }
  config.defineSimpleVar("CXXFLAGS", CXXFLAGS);

  const String pkgName = toMacroName(config.packageName);
  const Version& pkgVersion = getPackageVersion();
  String commitHash;
  String commitShortHash;
  String commitDate;
  try {
    git2::Repository repo{};
    repo.open(".");

    const git2::Oid oid = repo.refNameToId("HEAD");
    commitHash = oid.toString();
    commitShortHash = commitHash.substr(0, git2::SHORT_HASH_LEN);
    commitDate = git2::Commit().lookup(repo, oid).time().toString();
  } catch (const git2::Exception& e) {
    Logger::debug("No git repository found");
  }

  // Variables Poac sets for the user.
  addDefine(pkgName + "_PKG_NAME", config.packageName);
  addDefine(pkgName + "_PKG_VERSION", pkgVersion.toString());
  addDefine(pkgName + "_PKG_VERSION_MAJOR", std::to_string(pkgVersion.major));
  addDefine(pkgName + "_PKG_VERSION_MINOR", std::to_string(pkgVersion.minor));
  addDefine(pkgName + "_PKG_VERSION_PATCH", std::to_string(pkgVersion.patch));
  addDefine(pkgName + "_PKG_VERSION_PRE", pkgVersion.pre.toString());
  addDefine(pkgName + "_COMMIT_HASH", commitHash);
  addDefine(pkgName + "_COMMIT_SHORT_HASH", commitShortHash);
  addDefine(pkgName + "_COMMIT_DATE", commitDate);

  config.defineSimpleVar("DEFINES", DEFINES);
  config.defineSimpleVar("INCLUDES", INCLUDES);
  config.defineSimpleVar("LIBS", LIBS);
}

static BuildConfig
configureBuild(const bool isDebug) {
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
  config.setAll({ config.packageName });
  config.addPhony("all");

  Vec<Path> sourceFilePaths = listSourceFilePaths("src");
  String srcs;
  for (Path& sourceFilePath : sourceFilePaths) {
    sourceFilePath = PATH_FROM_OUT_DIR / sourceFilePath;
    srcs += ' ' + sourceFilePath.string();
  }
  config.defineSimpleVar("SRCS", srcs);

  // Source Pass
  HashSet<String> buildObjTargets;
  for (const Path& sourceFilePath : sourceFilePaths) {
    String objTarget; // source.o
    const HashSet<String> objTargetDeps =
        parseMMOutput(runMM(sourceFilePath), objTarget);

    const Path targetBaseDir = fs::relative(
        sourceFilePath.parent_path(), PATH_FROM_OUT_DIR / "src"_path
    );
    Path buildTargetBaseDir = config.buildOutDir;
    if (targetBaseDir != ".") {
      buildTargetBaseDir /= targetBaseDir;
    }

    const String buildObjTarget = buildTargetBaseDir / objTarget;
    buildObjTargets.insert(buildObjTarget);
    defineCompileTarget(config, buildObjTarget, sourceFilePath, objTargetDeps);
  }
  // Project binary target.
  const String mainObjTarget = config.buildOutDir / "main.o";
  HashSet<String> projTargetDeps = { mainObjTarget };
  collectBinDepObjs(
      projTargetDeps, "",
      config.targets.at(mainObjTarget).remDeps, // we don't need sourceFile
      buildObjTargets, config
  );
  defineLinkTarget(config, config.packageName, projTargetDeps);

  // Test Pass
  bool enableTesting = false;
  Vec<String> testCommands;
  HashSet<String> testTargets;
  for (const Path& sourceFilePath : sourceFilePaths) {
    if (!containsTestCode(sourceFilePath.string().substr(PATH_FROM_OUT_DIR.size(
        )))) {
      continue;
    }
    enableTesting = true;

    String objTarget; // source.o
    const HashSet<String> objTargetDeps =
        parseMMOutput(runMM(sourceFilePath, true /* isTest */), objTarget);

    const Path targetBaseDir = fs::relative(
        sourceFilePath.parent_path(), PATH_FROM_OUT_DIR / "src"_path
    );
    Path testTargetBaseDir = TEST_OUT_DIR;
    if (targetBaseDir != ".") {
      testTargetBaseDir /= targetBaseDir;
    }

    const String testObjTarget =
        (testTargetBaseDir / "test_").string() + objTarget;
    const String testTargetName = sourceFilePath.stem().string();
    const String testTarget =
        (testTargetBaseDir / "test_").string() + testTargetName;

    // Test object target.
    defineCompileTarget(
        config, testObjTarget, sourceFilePath, objTargetDeps, true /* isTest */
    );

    // Test binary target.
    HashSet<String> testTargetDeps = { testObjTarget };
    collectBinDepObjs(
        testTargetDeps, sourceFilePath.stem().string(), objTargetDeps,
        buildObjTargets, config
    );
    defineLinkTarget(config, testTarget, testTargetDeps);

    testCommands.emplace_back(printfCmd("Testing", testTargetName));
    testCommands.emplace_back(testTarget);
    testTargets.insert(testTarget);
  }
  if (enableTesting) {
    config.defineTarget("test", testCommands, testTargets);
    config.addPhony("test");
  }

  // Tidy Pass
  config.defineCondVar("POAC_TIDY", "clang-tidy");
  config.defineSimpleVar(
      "TIDY_TARGETS", "$(patsubst %,tidy_%,$(SRCS))", { "SRCS" }
  );
  config.defineTarget("tidy", {}, { "$(TIDY_TARGETS)" });
  config.defineTarget(
      "$(TIDY_TARGETS)",
      { "$(POAC_TIDY) $(POAC_TIDY_FLAGS) $< -- $(CXXFLAGS) "
        "$(DEFINES) -DPOAC_TEST $(INCLUDES)" },
      { "tidy_%:", "%" }
  );
  config.addPhony("tidy");
  config.addPhony("$(TIDY_TARGETS)");

  return config;
}

/// @returns the directory where the Makefile is generated.
String
emitMakefile(const bool debug) {
  setOutDir(debug);

  // When emitting Makefile, we also build the project.  So, we need to
  // make sure the dependencies are installed.
  installDeps();

  const String outDir = getOutDir();
  const String makefilePath = outDir + "/Makefile";
  if (isUpToDate(makefilePath)) {
    Logger::debug("Makefile is up to date");
    return outDir;
  }
  Logger::debug("Makefile is NOT up to date");

  const BuildConfig config = configureBuild(debug);
  std::ofstream ofs(makefilePath);
  config.emitMakefile(ofs);
  return outDir;
}

/// @returns the directory where the compilation database is generated.
String
emitCompdb(const bool debug) {
  setOutDir(debug);

  // compile_commands.json also needs INCLUDES, but not LIBS.
  installDeps();

  const String outDir = getOutDir();
  const String compdbPath = outDir + "/compile_commands.json";
  if (isUpToDate(compdbPath)) {
    Logger::debug("compile_commands.json is up to date");
    return outDir;
  }
  Logger::debug("compile_commands.json is NOT up to date");

  const BuildConfig config = configureBuild(debug);
  std::ofstream ofs(compdbPath);
  config.emitCompdb(outDir, ofs);
  return outDir;
}

String
modeString(const bool debug) {
  return debug ? "debug" : "release";
}

String
getMakeCommand(const bool isParallel) {
  String makeCommand;
  if (isVerbose()) {
    makeCommand = "make";
  } else {
    makeCommand = "make -s --no-print-directory Q=@";
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

namespace tests {

void
testCycleVars() {
  BuildConfig config;
  config.defineSimpleVar("a", "b", { "b" });
  config.defineSimpleVar("b", "c", { "c" });
  config.defineSimpleVar("c", "a", { "a" });

  assertException<PoacError>(
      [&config]() {
        std::ostringstream oss;
        config.emitMakefile(oss);
      },
      "too complex build graph"
  );

  pass();
}

void
testSimpleVars() {
  BuildConfig config;
  config.defineSimpleVar("c", "3", { "b" });
  config.defineSimpleVar("b", "2", { "a" });
  config.defineSimpleVar("a", "1");

  std::ostringstream oss;
  config.emitMakefile(oss);

  assertEq(
      oss.str(),
      "a := 1\n"
      "b := 2\n"
      "c := 3\n"
  );

  pass();
}

void
testDependOnUnregisteredVar() {
  BuildConfig config;
  config.defineSimpleVar("a", "1", { "b" });

  std::ostringstream oss;
  config.emitMakefile(oss);

  assertEq(oss.str(), "a := 1\n");

  pass();
}

void
testCycleTargets() {
  BuildConfig config;
  config.defineTarget("a", { "echo a" }, { "b" });
  config.defineTarget("b", { "echo b" }, { "c" });
  config.defineTarget("c", { "echo c" }, { "a" });

  assertException<PoacError>(
      [&config]() {
        std::ostringstream oss;
        config.emitMakefile(oss);
      },
      "too complex build graph"
  );

  pass();
}

void
testSimpleTargets() {
  BuildConfig config;
  config.defineTarget("a", { "echo a" });
  config.defineTarget("b", { "echo b" }, { "a" });
  config.defineTarget("c", { "echo c" }, { "b" });

  std::ostringstream oss;
  config.emitMakefile(oss);

  assertEq(
      oss.str(),
      "c: b\n"
      "\t$(Q)echo c\n"
      "\n"
      "b: a\n"
      "\t$(Q)echo b\n"
      "\n"
      "a:\n"
      "\t$(Q)echo a\n"
      "\n"
  );

  pass();
}

void
testDependOnUnregisteredTarget() {
  BuildConfig config;
  config.defineTarget("a", { "echo a" }, { "b" });

  std::ostringstream oss;
  config.emitMakefile(oss);

  assertEq(
      oss.str(),
      "a: b\n"
      "\t$(Q)echo a\n"
      "\n"
  );

  pass();
}

} // namespace tests

int
main() {
  tests::testCycleVars();
  tests::testSimpleVars();
  tests::testDependOnUnregisteredVar();
  tests::testCycleTargets();
  tests::testSimpleTargets();
  tests::testDependOnUnregisteredTarget();
}
#endif
