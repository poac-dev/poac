#pragma once

#include "Command.hpp"
#include "Exception.hpp"
#include "Rustify.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <tbb/spin_mutex.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// clang-format off
inline const std::unordered_set<std::string> SOURCE_FILE_EXTS{
  ".c", ".c++", ".cc", ".cpp", ".cxx"
};
inline const std::unordered_set<std::string> HEADER_FILE_EXTS{
  ".h", ".h++", ".hh", ".hpp", ".hxx"
};
// clang-format on

enum class VarType : uint8_t {
  Recursive, // =
  Simple, // :=
  Cond, // ?=
  Append, // +=
  Shell, // !=
};

struct Variable {
  std::string mValue;
  VarType mType = VarType::Simple;
};

struct Target {
  std::vector<std::string> mCommands;
  std::optional<std::string> mSourceFile;
  std::unordered_set<std::string> mRemDeps;
};

struct BuildConfig {
  // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)
  fs::path mOutBasePath;

private:
  std::string mPackageName;
  fs::path mBuildOutPath;
  fs::path mUnittestOutPath;
  bool mIsDebug{};

  std::unordered_map<std::string, Variable> mVariables;
  std::unordered_map<std::string, std::vector<std::string>> mVarDeps;
  std::unordered_map<std::string, Target> mTargets;
  std::unordered_map<std::string, std::vector<std::string>> mTargetDeps;
  std::optional<std::unordered_set<std::string>> mPhony;
  std::optional<std::unordered_set<std::string>> mAll;

  std::string mCxx;
  std::vector<std::string> mCxxflags;
  std::vector<std::string> mDefines;
  std::vector<std::string> mIncludes = { "-I../../include" };
  std::vector<std::string> mLibs;

public:
  explicit BuildConfig(const std::string& packageName, bool isDebug = true);

  void defineVar(
      const std::string& name, const Variable& value,
      const std::unordered_set<std::string>& dependsOn = {}
  ) {
    mVariables[name] = value;
    for (const std::string& dep : dependsOn) {
      // reverse dependency
      mVarDeps[dep].push_back(name);
    }
  }
  void defineSimpleVar(
      const std::string& name, const std::string& value,
      const std::unordered_set<std::string>& dependsOn = {}
  ) {
    defineVar(name, { .mValue = value, .mType = VarType::Simple }, dependsOn);
  }
  void defineCondVar(
      const std::string& name, const std::string& value,
      const std::unordered_set<std::string>& dependsOn = {}
  ) {
    defineVar(name, { .mValue = value, .mType = VarType::Cond }, dependsOn);
  }

  void defineTarget(
      const std::string& name, const std::vector<std::string>& commands,
      const std::unordered_set<std::string>& remDeps = {},
      const std::optional<std::string>& sourceFile = std::nullopt
  ) {
    mTargets[name] = { .mCommands = commands,
                       .mSourceFile = sourceFile,
                       .mRemDeps = remDeps };

    if (sourceFile.has_value()) {
      mTargetDeps[sourceFile.value()].push_back(name);
    }
    for (const std::string& dep : remDeps) {
      // reverse dependency
      mTargetDeps[dep].push_back(name);
    }
  }

  void addPhony(const std::string& target) {
    if (!mPhony.has_value()) {
      mPhony = { target };
    } else {
      mPhony->insert(target);
    }
  }

  void setAll(const std::unordered_set<std::string>& dependsOn) {
    mAll = dependsOn;
  }

  void emitVariable(std::ostream& os, const std::string& varName) const;
  void emitMakefile(std::ostream& os) const;
  void emitCompdb(std::ostream& os) const;
  std::string runMM(const std::string& sourceFile, bool isTest = false) const;
  bool containsTestCode(const std::string& sourceFile) const;

  void installDeps(bool includeDevDeps);
  void addDefine(std::string_view name, std::string_view value);
  void setVariables();

  void processSrc(
      const fs::path& sourceFilePath,
      std::unordered_set<std::string>& buildObjTargets,
      tbb::spin_mutex* mtx = nullptr
  );
  std::unordered_set<std::string>
  processSources(const std::vector<fs::path>& sourceFilePaths);

  void defineCompileTarget(
      const std::string& objTarget, const std::string& sourceFile,
      const std::unordered_set<std::string>& remDeps, bool isTest = false
  );
  void defineLinkTarget(
      const std::string& binTarget, const std::unordered_set<std::string>& deps
  );

  void collectBinDepObjs( // NOLINT(misc-no-recursion)
      std::unordered_set<std::string>& deps, std::string_view sourceFileName,
      const std::unordered_set<std::string>& objTargetDeps,
      const std::unordered_set<std::string>& buildObjTargets
  ) const;

  void processUnittestSrc(
      const fs::path& sourceFilePath,
      const std::unordered_set<std::string>& buildObjTargets,
      std::unordered_set<std::string>& testTargets,
      tbb::spin_mutex* mtx = nullptr
  );

  void configureBuild();
};

BuildConfig emitMakefile(bool isDebug, bool includeDevDeps);
std::string emitCompdb(bool isDebug, bool includeDevDeps);
std::string_view modeToString(bool isDebug);
std::string_view modeToProfile(bool isDebug);
Command getMakeCommand();
