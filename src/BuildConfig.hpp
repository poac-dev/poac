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

String emitMakefile(bool debug);
String emitCompdb(bool debug);
String modeString(bool debug);
String getMakeCommand(bool isParallel);
