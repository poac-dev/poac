#pragma once

#include "Rustify.hpp"

// clang-format off
inline const HashSet<String> SOURCE_FILE_EXTS{
  ".c", ".c++", ".cc", ".cpp", ".cxx"
};
inline const HashSet<String> HEADER_FILE_EXTS{
  ".h", ".h++", ".hh", ".hpp", ".hxx"
};
// clang-format on

String emitMakefile(bool isDebug, bool isParallel);
String emitCompdb(bool isDebug, bool isParallel);
String modeString(bool isDebug);
String getMakeCommand(bool isParallel);
