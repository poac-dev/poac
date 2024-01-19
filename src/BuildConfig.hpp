#pragma once

#include "Rustify.hpp"

// clang-format off
static inline const HashSet<String> SOURCE_FILE_EXTS{
  ".c", ".c++", ".cc", ".cpp", ".cxx"
};
static inline const HashSet<String> HEADER_FILE_EXTS{
  ".h", ".h++", ".hh", ".hpp", ".hxx"
};
// clang-format on

String emitMakefile(bool);
String emitCompdb(bool);
String modeString(bool);
String getMakeCommand(bool);
