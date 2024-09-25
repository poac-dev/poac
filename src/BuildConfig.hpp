#pragma once

#include "Command.hpp"

#include <string>
#include <unordered_set>

// clang-format off
inline const std::unordered_set<std::string> SOURCE_FILE_EXTS{
  ".c", ".c++", ".cc", ".cpp", ".cxx"
};
inline const std::unordered_set<std::string> HEADER_FILE_EXTS{
  ".h", ".h++", ".hh", ".hpp", ".hxx"
};
// clang-format on

std::string emitMakefile(bool isDebug, bool includeDevDeps);
std::string emitCompdb(bool isDebug, bool includeDevDeps);
std::string_view modeToString(bool isDebug);
std::string_view modeToProfile(bool isDebug);
Command getMakeCommand();
