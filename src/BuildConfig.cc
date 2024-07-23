#include "BuildConfig.hpp"

#include "Algos.hpp"
#include "Exception.hpp"
#include "Git2.hpp"
#include "Logger.hpp"
#include "Manifest.hpp"
#include "Parallelism.hpp"
#include "TermColor.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ostream>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/spin_mutex.h>
#include <thread>
#include <utility>
#include <vector>

static constinit const std::string_view TEST_OUT_DIR = "tests";
static constinit const std::string_view PATH_FROM_OUT_DIR = "../../";

static std::vector<fs::path>
listSourceFilePaths(const std::string_view directory) {
  std::vector<fs::path> sourceFilePaths;
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
  std::string value;
  VarType type = VarType::Simple;
};

std::ostream&
operator<<(std::ostream& os, const Variable& var) {
  os << var.type << ' ' << var.value;
  return os;
}

struct Target {
  std::vector<std::string> commands;
  Option<std::string> sourceFile;
  HashSet<std::string> remDeps;
};

struct BuildConfig {
  std::string packageName;
  fs::path buildOutDir;

  HashMap<std::string, Variable> variables;
  HashMap<std::string, std::vector<std::string>> varDeps;
  HashMap<std::string, Target> targets;
  HashMap<std::string, std::vector<std::string>> targetDeps;
  Option<HashSet<std::string>> phony;
  Option<HashSet<std::string>> all;

  std::string OUT_DIR;
  std::string CXX = "clang++";
  std::string CXXFLAGS;
  std::string DEFINES;
  std::string INCLUDES = " -Iinclude";
  std::string LIBS;

  BuildConfig() = default;
  explicit BuildConfig(const std::string& packageName)
      : packageName{ packageName }, buildOutDir{ packageName + ".d" } {}

  void setOutDir(const bool isDebug) {
    if (isDebug) {
      OUT_DIR = "poac-out/debug";
    } else {
      OUT_DIR = "poac-out/release";
    }
  }
  std::string getOutDir() const {
    if (OUT_DIR.empty()) {
      throw PoacError("outDir is not set");
    }
    return OUT_DIR;
  }

  void defineVar(
      const std::string& name, const Variable& value,
      const HashSet<std::string>& dependsOn = {}
  ) {
    variables[name] = value;
    for (const std::string& dep : dependsOn) {
      // reverse dependency
      varDeps[dep].push_back(name);
    }
  }
  void defineSimpleVar(
      const std::string& name, const std::string& value,
      const HashSet<std::string>& dependsOn = {}
  ) {
    defineVar(name, { value, VarType::Simple }, dependsOn);
  }
  void defineCondVar(
      const std::string& name, const std::string& value,
      const HashSet<std::string>& dependsOn = {}
  ) {
    defineVar(name, { value, VarType::Cond }, dependsOn);
  }

  void defineTarget(
      const std::string& name, const std::vector<std::string>& commands,
      const HashSet<std::string>& remDeps = {},
      const Option<std::string>& sourceFile = None
  ) {
    targets[name] = { commands, sourceFile, remDeps };

    if (sourceFile.has_value()) {
      targetDeps[sourceFile.value()].push_back(name);
    }
    for (const std::string& dep : remDeps) {
      // reverse dependency
      targetDeps[dep].push_back(name);
    }
  }

  void addPhony(const std::string& target) {
    if (!phony.has_value()) {
      phony = { target };
    } else {
      phony->insert(target);
    }
  }

  void setAll(const HashSet<std::string>& dependsOn) {
    all = dependsOn;
  }

  void emitVariable(std::ostream& os, const std::string& varName) const;
  void emitMakefile(std::ostream& os) const;
  void emitCompdb(std::string_view baseDir, std::ostream& os) const;
  std::string runMM(const std::string& sourceFile, bool isTest = false) const;
  bool containsTestCode(const std::string& sourceFile) const;

  void installDeps(bool includeDevDeps);
  void addDefine(std::string_view name, std::string_view value);
  void setVariables(bool isDebug);

  void processSrc(
      BuildConfig& config, const fs::path& sourceFilePath,
      HashSet<std::string>& buildObjTargets, tbb::spin_mutex* mtx = nullptr
  ) const;
};

static void
emitDep(std::ostream& os, usize& offset, const std::string_view dep) {
  constexpr usize maxLineLen = 80;
  if (offset + dep.size() + 2 > maxLineLen) { // 2 for space and \.
    // \ for line continuation. \ is the 80th character.
    os << std::setw(static_cast<int>(maxLineLen + 3 - offset)) << " \\\n ";
    offset = 2;
  }
  os << ' ' << dep;
  offset += dep.size() + 1; // space
}

static void
emitTarget(
    std::ostream& os, const std::string_view target,
    const HashSet<std::string>& dependsOn,
    const Option<std::string>& sourceFile = None,
    const std::vector<std::string>& commands = {}
) {
  usize offset = 0;

  os << target << ':';
  offset += target.size() + 2; // : and space

  if (sourceFile.has_value()) {
    emitDep(os, offset, sourceFile.value());
  }
  for (const std::string_view dep : dependsOn) {
    emitDep(os, offset, dep);
  }
  os << '\n';

  for (const std::string_view cmd : commands) {
    os << '\t';
    if (!cmd.starts_with('@')) {
      os << "$(Q)";
    }
    os << cmd << '\n';
  }
  os << '\n';
}

void
BuildConfig::emitVariable(std::ostream& os, const std::string& varName) const {
  std::ostringstream oss; // TODO: implement an elegant way to get type size.
  oss << varName << ' ' << variables.at(varName).type;
  const std::string left = oss.str();
  os << left << ' ';

  constexpr usize maxLineLen = 80; // TODO: share across sources?
  usize offset = left.size() + 1; // space
  std::string value;
  for (const char c : variables.at(varName).value) {
    if (c == ' ') {
      // Emit value
      if (offset + value.size() + 2 > maxLineLen) { // 2 for space and '\'
        os << std::setw(static_cast<int>(maxLineLen + 3 - offset)) << "\\\n  ";
        offset = 2;
      }
      os << value << ' ';
      offset += value.size() + 1;
      value.clear();
    } else {
      value.push_back(c);
    }
  }

  if (!value.empty()) {
    os << value;
  }
  os << '\n';
}

void
BuildConfig::emitMakefile(std::ostream& os) const {
  const std::vector<std::string> sortedVars = topoSort(variables, varDeps);
  for (const std::string& varName : sortedVars) {
    emitVariable(os, varName);
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
  const std::vector<std::string> sortedTargets = topoSort(targets, targetDeps);
  // NOLINTNEXTLINE(modernize-loop-convert)
  for (auto itr = sortedTargets.rbegin(); itr != sortedTargets.rend(); itr++) {
    emitTarget(
        os, *itr, targets.at(*itr).remDeps, targets.at(*itr).sourceFile,
        targets.at(*itr).commands
    );
  }
}

void
BuildConfig::emitCompdb(const std::string_view baseDir, std::ostream& os)
    const {
  const fs::path baseDirPath = fs::canonical(baseDir);
  const std::string indent1(2, ' ');
  const std::string indent2(4, ' ');

  std::ostringstream oss;
  for (const auto& [target, targetInfo] : targets) {
    if (phony->contains(target)) {
      // Ignore phony dependencies.
      continue;
    }

    bool isCompileTarget = false;
    for (const std::string_view cmd : targetInfo.commands) {
      if (!cmd.starts_with("$(CXX)") && !cmd.starts_with("@$(CXX)")) {
        continue;
      }
      if (cmd.find("-c") == std::string_view::npos) {
        // Ignore link commands.
        continue;
      }
      isCompileTarget = true;
    }
    if (!isCompileTarget) {
      continue;
    }

    // We don't check the Option value because we know the first dependency
    // always exists for compile targets.
    const std::string file = targetInfo.sourceFile.value();
    // The output is the target.
    const std::string output = target;
    std::string cmd = CXX;
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

  std::string output = oss.str();
  if (!output.empty()) {
    // Remove the last comma.
    output.pop_back(); // \n
    output.pop_back(); // ,
  }

  os << "[\n";
  os << output << '\n';
  os << "]\n";
}

std::string
BuildConfig::runMM(const std::string& sourceFile, const bool isTest) const {
  std::string command = "cd ";
  command += getOutDir();
  command += " && ";
  command += CXX;
  command += CXXFLAGS;
  command += DEFINES;
  command += INCLUDES;
  if (isTest) {
    command += " -DPOAC_TEST";
  }
  command += " -MM ";
  command += sourceFile;
  return getCmdOutput(command);
}

static HashSet<std::string>
parseMMOutput(const std::string& mmOutput, std::string& target) {
  std::istringstream iss(mmOutput);
  std::getline(iss, target, ':');

  std::string dependency;
  HashSet<std::string> deps;
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
isUpToDate(const std::string_view makefilePath) {
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

bool
BuildConfig::containsTestCode(const std::string& sourceFile) const {
  std::ifstream ifs(sourceFile);
  std::string line;
  while (std::getline(ifs, line)) {
    if (line.find("POAC_TEST") != std::string::npos) {
      // TODO: Can't we somehow elegantly make the compiler command sharable?
      std::string command = CXX;
      command += " -E ";
      command += CXXFLAGS;
      command += DEFINES;
      command += INCLUDES;
      command += ' ';
      command += sourceFile;

      const std::string src = getCmdOutput(command);

      command += " -DPOAC_TEST";
      const std::string testSrc = getCmdOutput(command);

      // If the source file contains POAC_TEST, by processing the source
      // file with -E, we can check if the source file contains POAC_TEST
      // or not semantically.  If the source file contains POAC_TEST, the
      // test source file should be different from the original source
      // file.
      const bool containsTest = src != testSrc;
      if (containsTest) {
        logger::debug("Found test code: ", sourceFile);
      }
      return containsTest;
    }
  }
  return false;
}

static std::string
printfCmd(const std::string_view header, const std::string_view body) {
  std::ostringstream oss;
  logger::info(oss, header, body);
  std::string msg = oss.str();

  // Replace all occurrences of '\n' with "\\n" to escape newlines
  size_t pos = 0;
  while ((pos = msg.find('\n', pos)) != std::string::npos) {
    msg.replace(pos, 1, "\\n");
    pos += 2; // Move past the replacement
  }

  return fmt::format("@printf '{}' >&2", msg);
}

static void
defineCompileTarget(
    BuildConfig& config, const std::string& objTarget,
    const std::string& sourceFile, const HashSet<std::string>& remDeps,
    const bool isTest = false
) {
  std::vector<std::string> commands;
  commands.push_back("@mkdir -p $(@D)");
  commands.push_back("$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES)");
  if (isTest) {
    commands.back() += " -DPOAC_TEST";
  }
  commands.back() += " -c $< -o $@";
  config.defineTarget(objTarget, commands, remDeps, sourceFile);
}

static void
defineLinkTarget(
    BuildConfig& config, const std::string& binTarget,
    const HashSet<std::string>& deps
) {
  std::vector<std::string> commands;
  commands.push_back("$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@");
  config.defineTarget(binTarget, commands, deps);
}

// Map a path to header file to the corresponding object file.
//
// e.g., src/path/to/header.h -> poac.d/path/to/header.o
static std::string
mapHeaderToObj(const fs::path& headerPath, const fs::path& buildOutDir) {
  fs::path objBaseDir =
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
collectBinDepObjs( // NOLINT(misc-no-recursion)
    HashSet<std::string>& deps, const std::string_view sourceFileName,
    const HashSet<std::string>& objTargetDeps,
    const HashSet<std::string>& buildObjTargets, const BuildConfig& config
) {
  for (const fs::path headerPath : objTargetDeps) {
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

    const std::string objTarget =
        mapHeaderToObj(headerPath, config.buildOutDir);
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

void
BuildConfig::installDeps(const bool includeDevDeps) {
  const std::vector<DepMetadata> deps = installDependencies(includeDevDeps);
  for (const DepMetadata& dep : deps) {
    INCLUDES += ' ' + dep.includes;
    LIBS += ' ' + dep.libs;
  }
  logger::debug("INCLUDES: ", INCLUDES);
  logger::debug("LIBS: ", LIBS);
}

void
BuildConfig::addDefine(
    const std::string_view name, const std::string_view value
) {
  DEFINES += fmt::format(" -D{}='\"{}\"'", name, value);
}

void
BuildConfig::setVariables(const bool isDebug) {
  this->defineCondVar("CXX", CXX);

  CXXFLAGS += " -std=c++" + getPackageEdition().getString();
  if (shouldColor()) {
    CXXFLAGS += " -fdiagnostics-color";
  }
  if (isDebug) {
    CXXFLAGS += " -g -O0 -DDEBUG";
  } else {
    CXXFLAGS += " -O3 -DNDEBUG";
  }
  const Profile& profile = isDebug ? getDevProfile() : getReleaseProfile();
  if (profile.lto) {
    CXXFLAGS += " -flto";
  }
  for (const std::string_view flag : profile.cxxflags) {
    CXXFLAGS += ' ';
    CXXFLAGS += flag;
  }
  this->defineSimpleVar("CXXFLAGS", CXXFLAGS);

  const std::string pkgName = toMacroName(this->packageName);
  const Version& pkgVersion = getPackageVersion();
  std::string commitHash;
  std::string commitShortHash;
  std::string commitDate;
  try {
    git2::Repository repo{};
    repo.open(".");

    const git2::Oid oid = repo.refNameToId("HEAD");
    commitHash = oid.toString();
    commitShortHash = commitHash.substr(0, git2::SHORT_HASH_LEN);
    commitDate = git2::Commit().lookup(repo, oid).time().toString();
  } catch (const git2::Exception& e) {
    logger::debug("No git repository found");
  }

  // Variables Poac sets for the user.
  const std::vector<std::pair<std::string, std::string>> defines{
    { fmt::format("POAC_{}_PKG_NAME", pkgName), this->packageName },
    { fmt::format("POAC_{}_PKG_VERSION", pkgName), pkgVersion.toString() },
    { fmt::format("POAC_{}_PKG_VERSION_MAJOR", pkgName),
      std::to_string(pkgVersion.major) },
    { fmt::format("POAC_{}_PKG_VERSION_MINOR", pkgName),
      std::to_string(pkgVersion.minor) },
    { fmt::format("POAC_{}_PKG_VERSION_PATCH", pkgName),
      std::to_string(pkgVersion.patch) },
    { fmt::format("POAC_{}_PKG_VERSION_PRE", pkgName),
      pkgVersion.pre.toString() },
    { fmt::format("POAC_{}_COMMIT_HASH", pkgName), commitHash },
    { fmt::format("POAC_{}_COMMIT_SHORT_HASH", pkgName), commitShortHash },
    { fmt::format("POAC_{}_COMMIT_DATE", pkgName), commitDate },
    { fmt::format("POAC_{}_PROFILE", pkgName),
      std::string(modeToString(isDebug)) },
  };
  for (const auto& [key, val] : defines) {
    addDefine(key, val);
  }

  this->defineSimpleVar("DEFINES", DEFINES);
  this->defineSimpleVar("INCLUDES", INCLUDES);
  this->defineSimpleVar("LIBS", LIBS);
}

void
BuildConfig::processSrc(
    BuildConfig& config, const fs::path& sourceFilePath,
    HashSet<std::string>& buildObjTargets, tbb::spin_mutex* mtx
) const {
  std::string objTarget; // source.o
  const HashSet<std::string> objTargetDeps =
      parseMMOutput(runMM(sourceFilePath), objTarget);

  const fs::path targetBaseDir = fs::relative(
      sourceFilePath.parent_path(), PATH_FROM_OUT_DIR / "src"_path
  );
  fs::path buildTargetBaseDir = config.buildOutDir;
  if (targetBaseDir != ".") {
    buildTargetBaseDir /= targetBaseDir;
  }

  const std::string buildObjTarget = buildTargetBaseDir / objTarget;

  if (mtx) {
    mtx->lock();
  }
  buildObjTargets.insert(buildObjTarget);
  defineCompileTarget(config, buildObjTarget, sourceFilePath, objTargetDeps);
  if (mtx) {
    mtx->unlock();
  }
}

static HashSet<std::string>
processSources(
    BuildConfig& config, const std::vector<fs::path>& sourceFilePaths
) {
  HashSet<std::string> buildObjTargets;

  if (isParallel()) {
    tbb::spin_mutex mtx;
    tbb::parallel_for(
        tbb::blocked_range<usize>(0, sourceFilePaths.size()),
        [&](const tbb::blocked_range<usize>& rng) {
          for (usize i = rng.begin(); i != rng.end(); ++i) {
            config.processSrc(
                config, sourceFilePaths[i], buildObjTargets, &mtx
            );
          }
        }
    );
  } else {
    for (const fs::path& sourceFilePath : sourceFilePaths) {
      config.processSrc(config, sourceFilePath, buildObjTargets);
    }
  }

  return buildObjTargets;
}

static void
processTestSrc(
    BuildConfig& config, const fs::path& sourceFilePath,
    const HashSet<std::string>& buildObjTargets,
    std::vector<std::string>& testCommands, HashSet<std::string>& testTargets,
    tbb::spin_mutex* mtx = nullptr
) {
  if (!config.containsTestCode(
          sourceFilePath.string().substr(PATH_FROM_OUT_DIR.size())
      )) {
    return;
  }

  std::string objTarget; // source.o
  const HashSet<std::string> objTargetDeps =
      parseMMOutput(config.runMM(sourceFilePath, /*isTest=*/true), objTarget);

  const fs::path targetBaseDir = fs::relative(
      sourceFilePath.parent_path(), PATH_FROM_OUT_DIR / "src"_path
  );
  fs::path testTargetBaseDir = TEST_OUT_DIR;
  if (targetBaseDir != ".") {
    testTargetBaseDir /= targetBaseDir;
  }

  const std::string testObjTarget =
      (testTargetBaseDir / "test_").string() + objTarget;
  const std::string testTargetName = sourceFilePath.stem().string();
  const std::string testTarget =
      (testTargetBaseDir / "test_").string() + testTargetName;

  // Test binary target.
  HashSet<std::string> testTargetDeps = { testObjTarget };
  collectBinDepObjs(
      testTargetDeps, sourceFilePath.stem().string(), objTargetDeps,
      buildObjTargets, config
  );

  if (mtx) {
    mtx->lock();
  }
  // Test object target.
  defineCompileTarget(
      config, testObjTarget, sourceFilePath, objTargetDeps, /*isTest=*/true
  );

  // Test binary target.
  defineLinkTarget(config, testTarget, testTargetDeps);

  testCommands.emplace_back(printfCmd(
      "Running",
      "unittests " + sourceFilePath.string().substr(PATH_FROM_OUT_DIR.size())
  ));
  testCommands.emplace_back(testTarget);
  testTargets.insert(testTarget);
  if (mtx) {
    mtx->unlock();
  }
}

static void
configureBuild(BuildConfig& config, const bool isDebug) {
  if (!fs::exists("src")) {
    throw PoacError("src directory not found");
  }
  if (!fs::exists("src/main.cc")) {
    // For now, we only support .cc extension only for the main file.
    throw PoacError("src/main.cc not found");
  }

  const std::string outDir = config.getOutDir();
  if (!fs::exists(outDir)) {
    fs::create_directories(outDir);
  }
  if (const char* cxx = std::getenv("CXX")) {
    config.CXX = cxx;
  }

  config.setVariables(isDebug);

  // Build rules
  config.setAll({ config.packageName + "_before" });
  config.addPhony("all");

  std::vector<fs::path> sourceFilePaths = listSourceFilePaths("src");
  std::string srcs;
  for (fs::path& sourceFilePath : sourceFilePaths) {
    sourceFilePath = PATH_FROM_OUT_DIR / sourceFilePath;
    srcs += ' ' + sourceFilePath.string();
  }
  config.defineSimpleVar("SRCS", srcs);

  // Source Pass
  const HashSet<std::string> buildObjTargets =
      processSources(config, sourceFilePaths);

  // Project binary target.
  const std::string mainObjTarget = config.buildOutDir / "main.o";
  HashSet<std::string> projTargetDeps = { mainObjTarget };
  collectBinDepObjs(
      projTargetDeps, "",
      config.targets.at(mainObjTarget).remDeps, // we don't need sourceFile
      buildObjTargets, config
  );
  config.defineTarget(
      config.packageName + "_before",
      { printfCmd("Compiling", config.packageName),
        "+make " + config.packageName }
  );
  config.addPhony(config.packageName + "_before");
  defineLinkTarget(config, config.packageName, projTargetDeps);

  // Test Pass
  std::vector<std::string> testCommands;
  HashSet<std::string> testTargets;
  if (isParallel()) {
    tbb::spin_mutex mtx;
    tbb::parallel_for(
        tbb::blocked_range<usize>(0, sourceFilePaths.size()),
        [&](const tbb::blocked_range<usize>& rng) {
          for (usize i = rng.begin(); i != rng.end(); ++i) {
            processTestSrc(
                config, sourceFilePaths[i], buildObjTargets, testCommands,
                testTargets, &mtx
            );
          }
        }
    );
  } else {
    for (const fs::path& sourceFilePath : sourceFilePaths) {
      processTestSrc(
          config, sourceFilePath, buildObjTargets, testCommands, testTargets
      );
    }
  }
  if (!testCommands.empty()) {
    config.defineTarget(
        "test",
        { printfCmd("Compiling", config.packageName), "+make test_inner" }
    );
    config.defineTarget("test_inner", testCommands, testTargets);
    config.addPhony("test");
    config.addPhony("test_inner");
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
      { "tidy_%: %" }
  );
  config.addPhony("tidy");
  config.addPhony("$(TIDY_TARGETS)");
}

/// @returns the directory where the Makefile is generated.
std::string
emitMakefile(const bool isDebug, const bool includeDevDeps) {
  BuildConfig config(getPackageName());
  config.setOutDir(isDebug);

  // When emitting Makefile, we also build the project.  So, we need to
  // make sure the dependencies are installed.
  config.installDeps(includeDevDeps);

  const std::string outDir = config.getOutDir();
  const std::string makefilePath = outDir + "/Makefile";
  if (isUpToDate(makefilePath)) {
    logger::debug("Makefile is up to date");
    return outDir;
  }
  logger::debug("Makefile is NOT up to date");

  configureBuild(config, isDebug);
  std::ofstream ofs(makefilePath);
  config.emitMakefile(ofs);
  return outDir;
}

/// @returns the directory where the compilation database is generated.
std::string
emitCompdb(const bool isDebug, const bool includeDevDeps) {
  BuildConfig config(getPackageName());
  config.setOutDir(isDebug);

  // compile_commands.json also needs INCLUDES, but not LIBS.
  config.installDeps(includeDevDeps);

  const std::string outDir = config.getOutDir();
  const std::string compdbPath = outDir + "/compile_commands.json";
  if (isUpToDate(compdbPath)) {
    logger::debug("compile_commands.json is up to date");
    return outDir;
  }
  logger::debug("compile_commands.json is NOT up to date");

  configureBuild(config, isDebug);
  std::ofstream ofs(compdbPath);
  config.emitCompdb(outDir, ofs);
  return outDir;
}

std::string_view
modeToString(const bool isDebug) {
  return isDebug ? "debug" : "release";
}

std::string_view
modeToProfile(const bool isDebug) {
  return isDebug ? "dev" : "release";
}

std::string
getMakeCommand() {
  std::string makeCommand;
  if (isVerbose()) {
    makeCommand = "make";
  } else {
    makeCommand = "make -s --no-print-directory Q=@";
  }

  const usize numThreads = getParallelism();
  if (numThreads > 1) {
    makeCommand += " -j" + std::to_string(numThreads);
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
