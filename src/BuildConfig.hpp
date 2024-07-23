#pragma once

#include "Rustify.hpp"

#include <string>

// clang-format off
inline const HashSet<std::string> SOURCE_FILE_EXTS{
  ".c", ".c++", ".cc", ".cpp", ".cxx"
};
inline const HashSet<std::string> HEADER_FILE_EXTS{
  ".h", ".h++", ".hh", ".hpp", ".hxx"
};
// clang-format on

std::string emitMakefile(bool isDebug, bool includeDevDeps);
std::string emitCompdb(bool isDebug, bool includeDevDeps);
std::string_view modeToString(bool isDebug);
std::string_view modeToProfile(bool isDebug);
std::string getMakeCommand();
