#pragma once

#include "Rustify.hpp"

static inline const HashSet<String> SOURCE_FILE_EXTS{
    ".c", ".c++", ".cc", ".cpp", ".cxx"
};
static inline const HashSet<String> HEADER_FILE_EXTS{
    ".h", ".h++", ".hh", ".hpp", ".hxx"
};

bool isDebugMode(StringRef);
String emitMakefile(const bool);
String modeString(const bool);
String getMakeCommand();
