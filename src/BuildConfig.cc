#include "BuildConfig.hpp"

#include "Algos.hpp"
#include "Command.hpp"
#include "Exception.hpp"
#include "Git2.hpp"
#include "Logger.hpp"
#include "Manifest.hpp"
#include "Parallelism.hpp"
#include "TermColor.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <queue>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/spin_mutex.h>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

constinit const std::string_view UNITTEST_OUT_DIR = "unittests";
constinit const std::string_view PATH_FROM_OUT_DIR = "../../";

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

BuildConfig::BuildConfig(const std::string& packageName, const bool isDebug)
    : packageName{ packageName }, buildOutDir{ packageName + ".d" },
      isDebug{ isDebug } {
  if (isDebug) {
    outputBasePath = getProjectBasePath() / "poac-out" / "debug";
  } else {
    outputBasePath = getProjectBasePath() / "poac-out" / "release";
  }

  if (const char* cxx = std::getenv("CXX")) {
    this->cxx = cxx;
  } else {
    const std::string output = Command("make")
                                   .addArg("--print-data-base")
                                   .addArg("--question")
                                   .addArg("-f")
                                   .addArg("/dev/null")
                                   .setStderrConfig(Command::IOConfig::Null)
                                   .output()
                                   .stdout;
    std::istringstream iss(output);
    std::string line;

    while (std::getline(iss, line)) {
      if (line.starts_with("CXX = ")) {
        this->cxx = line.substr("CXX = "sv.size());
        return;
      }
    }
    throw PoacError("failed to get CXX from make");
  }
}

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
    const std::unordered_set<std::string>& dependsOn,
    const std::optional<std::string>& sourceFile = std::nullopt,
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

template <typename T>
std::vector<std::string>
topoSort(
    const std::unordered_map<std::string, T>& list,
    const std::unordered_map<std::string, std::vector<std::string>>& adjList
) {
  std::unordered_map<std::string, u32> inDegree;
  for (const auto& var : list) {
    inDegree[var.first] = 0;
  }
  for (const auto& edge : adjList) {
    if (!list.contains(edge.first)) {
      continue; // Ignore nodes not in list
    }
    if (!inDegree.contains(edge.first)) {
      inDegree[edge.first] = 0;
    }
    for (const auto& neighbor : edge.second) {
      inDegree[neighbor]++;
    }
  }

  std::queue<std::string> zeroInDegree;
  for (const auto& var : inDegree) {
    if (var.second == 0) {
      zeroInDegree.push(var.first);
    }
  }

  std::vector<std::string> res;
  while (!zeroInDegree.empty()) {
    const std::string node = zeroInDegree.front();
    zeroInDegree.pop();
    res.push_back(node);

    if (!adjList.contains(node)) {
      // No dependencies
      continue;
    }
    for (const std::string& neighbor : adjList.at(node)) {
      inDegree[neighbor]--;
      if (inDegree[neighbor] == 0) {
        zeroInDegree.push(neighbor);
      }
    }
  }

  if (res.size() != list.size()) {
    // Cycle detected
    throw PoacError("too complex build graph");
  }
  return res;
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
  for (const auto& sortedTarget : std::ranges::reverse_view(sortedTargets)) {
    emitTarget(
        os, sortedTarget, targets.at(sortedTarget).remDeps,
        targets.at(sortedTarget).sourceFile, targets.at(sortedTarget).commands
    );
  }
}

void
BuildConfig::emitCompdb(const fs::path& basePath, std::ostream& os)
    const {
  const fs::path canonBasePath = fs::canonical(basePath);
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

    // We don't check the std::optional value because we know the first
    // dependency always exists for compile targets.
    const std::string file = targetInfo.sourceFile.value();
    // The output is the target.
    const std::string output = target;
    const Command cmd = Command(cxx)
                            .addArgs(cxxflags)
                            .addArgs(defines)
                            .addArg("-DPOAC_TEST")
                            .addArgs(includes)
                            .addArg("-c")
                            .addArg(file)
                            .addArg("-o")
                            .addArg(output);

    oss << indent1 << "{\n";
    oss << indent2 << "\"directory\": " << canonBasePath << ",\n";
    oss << indent2 << "\"file\": " << std::quoted(file) << ",\n";
    oss << indent2 << "\"output\": " << std::quoted(output) << ",\n";
    oss << indent2 << "\"command\": " << std::quoted(cmd.toString()) << "\n";
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
  Command command =
      Command(cxx).addArgs(cxxflags).addArgs(defines).addArgs(includes);
  if (isTest) {
    command.addArg("-DPOAC_TEST");
  }
  command.addArg("-MM");
  command.addArg(sourceFile);
  command.setWorkingDirectory(outputBasePath);
  return getCmdOutput(command);
}

static std::unordered_set<std::string>
parseMMOutput(const std::string& mmOutput, std::string& target) {
  std::istringstream iss(mmOutput);
  std::getline(iss, target, ':');

  std::string dependency;
  std::unordered_set<std::string> deps;
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
  const fs::path srcDir = getProjectBasePath() / "src";
  for (const auto& entry : fs::recursive_directory_iterator(srcDir)) {
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
      Command command(cxx);
      command.addArg("-E");
      command.addArgs(cxxflags);
      command.addArgs(defines);
      command.addArgs(includes);
      command.addArg(sourceFile);

      const std::string src = getCmdOutput(command);

      command.addArg("-DPOAC_TEST");
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

void
BuildConfig::defineCompileTarget(
    const std::string& objTarget, const std::string& sourceFile,
    const std::unordered_set<std::string>& remDeps, const bool isTest
) {
  std::vector<std::string> commands;
  commands.emplace_back("@mkdir -p $(@D)");
  commands.emplace_back("$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES)");
  if (isTest) {
    commands.back() += " -DPOAC_TEST";
  }
  commands.back() += " -c $< -o $@";
  defineTarget(objTarget, commands, remDeps, sourceFile);
}

void
BuildConfig::defineLinkTarget(
    const std::string& binTarget, const std::unordered_set<std::string>& deps
) {
  std::vector<std::string> commands;
  commands.emplace_back("$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@");
  defineTarget(binTarget, commands, deps);
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
void
BuildConfig::collectBinDepObjs( // NOLINT(misc-no-recursion)
    std::unordered_set<std::string>& deps,
    const std::string_view sourceFileName,
    const std::unordered_set<std::string>& objTargetDeps,
    const std::unordered_set<std::string>& buildObjTargets
) const {
  for (const fs::path headerPath : objTargetDeps) {
    if (sourceFileName == headerPath.stem()) {
      // We shouldn't depend on the original object file (e.g.,
      // poac.d/path/to/file.o). We should depend on the test object
      // file (e.g., unittests/path/to/file.o).
      continue;
    }
    if (!HEADER_FILE_EXTS.contains(headerPath.extension())) {
      // We only care about header files.
      continue;
    }

    const std::string objTarget = mapHeaderToObj(headerPath, buildOutDir);
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
        targets.at(objTarget).remDeps, // we don't need sourceFile
        buildObjTargets
    );
  }
}

void
BuildConfig::installDeps(const bool includeDevDeps) {
  const std::vector<DepMetadata> deps = installDependencies(includeDevDeps);
  for (const DepMetadata& dep : deps) {
    if (!dep.includes.empty()) {
      includes.push_back(replaceAll(dep.includes, "-I", "-isystem"));
    }
    if (!dep.libs.empty()) {
      libs.push_back(dep.libs);
    }
  }
  logger::debug(fmt::format("INCLUDES: {}", includes));
  logger::debug(fmt::format("LIBS: {}", libs));
}

void
BuildConfig::addDefine(
    const std::string_view name, const std::string_view value
) {
  defines.push_back(fmt::format("-D{}='\"{}\"'", name, value));
}

void
BuildConfig::setVariables() {
  this->defineSimpleVar("CXX", cxx);

  cxxflags.push_back("-std=c++" + getPackageEdition().getString());
  if (shouldColor()) {
    cxxflags.emplace_back("-fdiagnostics-color");
  }
  if (isDebug) {
    cxxflags.emplace_back("-g");
    cxxflags.emplace_back("-O0");
    cxxflags.emplace_back("-DDEBUG");
  } else {
    cxxflags.emplace_back("-O3");
    cxxflags.emplace_back("-DNDEBUG");
  }
  const Profile& profile = isDebug ? getDevProfile() : getReleaseProfile();
  if (profile.lto) {
    cxxflags.emplace_back("-flto");
  }
  for (const std::string_view flag : profile.cxxflags) {
    cxxflags.emplace_back(flag);
  }
  this->defineSimpleVar(
      "CXXFLAGS", fmt::format("{:s}", fmt::join(cxxflags, " "))
  );

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

  this->defineSimpleVar(
      "DEFINES", fmt::format("{:s}", fmt::join(this->defines, " "))
  );
  this->defineSimpleVar(
      "INCLUDES", fmt::format("{:s}", fmt::join(includes, " "))
  );
  this->defineSimpleVar("LIBS", fmt::format("{:s}", fmt::join(libs, " ")));
}

void
BuildConfig::processSrc(
    const fs::path& sourceFilePath,
    std::unordered_set<std::string>& buildObjTargets, tbb::spin_mutex* mtx
) {
  std::string objTarget; // source.o
  const std::unordered_set<std::string> objTargetDeps =
      parseMMOutput(runMM(sourceFilePath), objTarget);

  const fs::path targetBaseDir = fs::relative(
      sourceFilePath.parent_path(), PATH_FROM_OUT_DIR / "src"_path
  );
  fs::path buildTargetBaseDir = buildOutDir;
  if (targetBaseDir != ".") {
    buildTargetBaseDir /= targetBaseDir;
  }

  const std::string buildObjTarget = buildTargetBaseDir / objTarget;

  if (mtx) {
    mtx->lock();
  }
  buildObjTargets.insert(buildObjTarget);
  defineCompileTarget(buildObjTarget, sourceFilePath, objTargetDeps);
  if (mtx) {
    mtx->unlock();
  }
}

std::unordered_set<std::string>
BuildConfig::processSources(const std::vector<fs::path>& sourceFilePaths) {
  std::unordered_set<std::string> buildObjTargets;

  if (isParallel()) {
    tbb::spin_mutex mtx;
    tbb::parallel_for(
        tbb::blocked_range<usize>(0, sourceFilePaths.size()),
        [&](const tbb::blocked_range<usize>& rng) {
          for (usize i = rng.begin(); i != rng.end(); ++i) {
            processSrc(sourceFilePaths[i], buildObjTargets, &mtx);
          }
        }
    );
  } else {
    for (const fs::path& sourceFilePath : sourceFilePaths) {
      processSrc(sourceFilePath, buildObjTargets);
    }
  }

  return buildObjTargets;
}

void
BuildConfig::processUnittestSrc(
    const fs::path& sourceFilePath,
    const std::unordered_set<std::string>& buildObjTargets,
    std::unordered_set<std::string>& testTargets, tbb::spin_mutex* mtx
) {
  if (!containsTestCode(sourceFilePath.string().substr(PATH_FROM_OUT_DIR.size())
      )) {
    return;
  }

  std::string objTarget; // source.o
  const std::unordered_set<std::string> objTargetDeps =
      parseMMOutput(runMM(sourceFilePath, /*isTest=*/true), objTarget);

  const fs::path targetBaseDir = fs::relative(
      sourceFilePath.parent_path(), PATH_FROM_OUT_DIR / "src"_path
  );
  fs::path testTargetBaseDir = UNITTEST_OUT_DIR;
  if (targetBaseDir != ".") {
    testTargetBaseDir /= targetBaseDir;
  }

  const std::string testObjTarget = testTargetBaseDir / objTarget;
  const std::string testTarget =
      (testTargetBaseDir / sourceFilePath.filename()).string() + ".test";

  // Test binary target.
  std::unordered_set<std::string> testTargetDeps = { testObjTarget };
  collectBinDepObjs(
      testTargetDeps, sourceFilePath.stem().string(), objTargetDeps,
      buildObjTargets
  );

  if (mtx) {
    mtx->lock();
  }
  // Test object target.
  defineCompileTarget(
      testObjTarget, sourceFilePath, objTargetDeps, /*isTest=*/true
  );

  // Test binary target.
  defineLinkTarget(testTarget, testTargetDeps);

  testTargets.insert(testTarget);
  if (mtx) {
    mtx->unlock();
  }
}

static std::vector<fs::path>
listSourceFilePaths(const fs::path& dir) {
  std::vector<fs::path> sourceFilePaths;
  for (const auto& entry : fs::recursive_directory_iterator(dir)) {
    if (!SOURCE_FILE_EXTS.contains(entry.path().extension())) {
      continue;
    }
    sourceFilePaths.emplace_back(entry.path());
  }
  return sourceFilePaths;
}

void
BuildConfig::configureBuild() {
  const fs::path srcDir = getProjectBasePath() / "src";
  if (!fs::exists(srcDir)) {
    throw PoacError(srcDir, " is required but not found");
  }

  // find main source file
  const auto isMainSource = [](const fs::path& file) {
    return file.filename().stem() == "main";
  };
  fs::path mainSource;
  for (const auto& entry : fs::directory_iterator(srcDir)) {
    const fs::path& path = entry.path();
    if (!SOURCE_FILE_EXTS.contains(path.extension())) {
      continue;
    }
    if (!isMainSource(path)) {
      continue;
    }
    if (mainSource.empty()) {
      mainSource = path;
    } else {
      throw PoacError("multiple main sources were found");
    }
  }

  if (mainSource.empty()) {
    throw PoacError(fmt::format("src/main{} was not found", SOURCE_FILE_EXTS));
  }

  if (!fs::exists(outputBasePath)) {
    fs::create_directories(outputBasePath);
  }

  setVariables();

  // Build rules
  setAll({ packageName });
  addPhony("all");

  std::vector<fs::path> sourceFilePaths = listSourceFilePaths(srcDir);
  std::string srcs;
  for (fs::path& sourceFilePath : sourceFilePaths) {
    if (sourceFilePath != mainSource && isMainSource(sourceFilePath)) {
      logger::warn(fmt::format(
          "source file `{}` is named `main` but is not located directly in the "
          "`src/` directory. "
          "This file will not be treated as the program's entry point. "
          "Move it directly to 'src/' if intended as such.",
          sourceFilePath.string()
      ));
    }

    sourceFilePath = PATH_FROM_OUT_DIR / sourceFilePath;
    srcs += ' ' + sourceFilePath.string();
  }

  defineSimpleVar("SRCS", srcs);

  // Source Pass
  const std::unordered_set<std::string> buildObjTargets =
      processSources(sourceFilePaths);

  // Project binary target.
  const std::string mainObjTarget = buildOutDir / "main.o";
  std::unordered_set<std::string> projTargetDeps = { mainObjTarget };
  collectBinDepObjs(
      projTargetDeps, "",
      targets.at(mainObjTarget).remDeps, // we don't need sourceFile
      buildObjTargets
  );
  defineLinkTarget(packageName, projTargetDeps);

  // Test Pass
  std::unordered_set<std::string> testTargets;
  if (isParallel()) {
    tbb::spin_mutex mtx;
    tbb::parallel_for(
        tbb::blocked_range<usize>(0, sourceFilePaths.size()),
        [&](const tbb::blocked_range<usize>& rng) {
          for (usize i = rng.begin(); i != rng.end(); ++i) {
            processUnittestSrc(
                sourceFilePaths[i], buildObjTargets, testTargets, &mtx
            );
          }
        }
    );
  } else {
    for (const fs::path& sourceFilePath : sourceFilePaths) {
      processUnittestSrc(sourceFilePath, buildObjTargets, testTargets);
    }
  }

  // Tidy Pass
  defineCondVar("POAC_TIDY", "clang-tidy");
  defineSimpleVar("TIDY_TARGETS", "$(patsubst %,tidy_%,$(SRCS))", { "SRCS" });
  defineTarget("tidy", {}, { "$(TIDY_TARGETS)" });
  defineTarget(
      "$(TIDY_TARGETS)",
      { "$(POAC_TIDY) $(POAC_TIDY_FLAGS) $< -- $(CXXFLAGS) "
        "$(DEFINES) -DPOAC_TEST $(INCLUDES)" },
      { "tidy_%: %" }
  );
  addPhony("tidy");
  addPhony("$(TIDY_TARGETS)");
}

BuildConfig
emitMakefile(const bool isDebug, const bool includeDevDeps) {
  BuildConfig config(getPackageName(), isDebug);

  // When emitting Makefile, we also build the project.  So, we need to
  // make sure the dependencies are installed.
  config.installDeps(includeDevDeps);

  const std::string makefilePath = config.outputBasePath / "Makefile";
  if (isUpToDate(makefilePath)) {
    logger::debug("Makefile is up to date");
    return config;
  }
  logger::debug("Makefile is NOT up to date");

  config.configureBuild();
  std::ofstream ofs(makefilePath);
  config.emitMakefile(ofs);
  return config;
}

/// @returns the directory where the compilation database is generated.
std::string
emitCompdb(const bool isDebug, const bool includeDevDeps) {
  BuildConfig config(getPackageName(), isDebug);

  // compile_commands.json also needs INCLUDES, but not LIBS.
  config.installDeps(includeDevDeps);

  const std::string compdbPath = config.outputBasePath / "compile_commands.json";
  if (isUpToDate(compdbPath)) {
    logger::debug("compile_commands.json is up to date");
    return config.outputBasePath;
  }
  logger::debug("compile_commands.json is NOT up to date");

  config.configureBuild();
  std::ofstream ofs(compdbPath);
  config.emitCompdb(config.outputBasePath, ofs);
  return config.outputBasePath;
}

std::string_view
modeToString(const bool isDebug) {
  return isDebug ? "debug" : "release";
}

std::string_view
modeToProfile(const bool isDebug) {
  return isDebug ? "dev" : "release";
}

Command
getMakeCommand() {
  Command makeCommand("make");
  if (!isVerbose()) {
    makeCommand.addArg("-s").addArg("--no-print-directory").addArg("Q=@");
  }
  if (isQuiet()) {
    makeCommand.addArg("QUIET=1");
  }

  const usize numThreads = getParallelism();
  if (numThreads > 1) {
    makeCommand.addArg("-j" + std::to_string(numThreads));
  }

  return makeCommand;
}

#ifdef POAC_TEST

namespace tests {

static void
testCycleVars() {
  BuildConfig config("test");
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

static void
testSimpleVars() {
  BuildConfig config("test");
  config.defineSimpleVar("c", "3", { "b" });
  config.defineSimpleVar("b", "2", { "a" });
  config.defineSimpleVar("a", "1");

  std::ostringstream oss;
  config.emitMakefile(oss);

  assertTrue(
      oss.str().starts_with("a := 1\n"
                            "b := 2\n"
                            "c := 3\n")
  );

  pass();
}

static void
testDependOnUnregisteredVar() {
  BuildConfig config("test");
  config.defineSimpleVar("a", "1", { "b" });

  std::ostringstream oss;
  config.emitMakefile(oss);

  assertTrue(oss.str().starts_with("a := 1\n"));

  pass();
}

static void
testCycleTargets() {
  BuildConfig config("test");
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

static void
testSimpleTargets() {
  BuildConfig config("test");
  config.defineTarget("a", { "echo a" });
  config.defineTarget("b", { "echo b" }, { "a" });
  config.defineTarget("c", { "echo c" }, { "b" });

  std::ostringstream oss;
  config.emitMakefile(oss);

  assertTrue(
      oss.str().ends_with("c: b\n"
                          "\t$(Q)echo c\n"
                          "\n"
                          "b: a\n"
                          "\t$(Q)echo b\n"
                          "\n"
                          "a:\n"
                          "\t$(Q)echo a\n"
                          "\n")
  );

  pass();
}

static void
testDependOnUnregisteredTarget() {
  BuildConfig config("test");
  config.defineTarget("a", { "echo a" }, { "b" });

  std::ostringstream oss;
  config.emitMakefile(oss);

  assertTrue(
      oss.str().ends_with("a: b\n"
                          "\t$(Q)echo a\n"
                          "\n")
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
