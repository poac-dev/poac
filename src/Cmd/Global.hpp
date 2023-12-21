#pragma once

#include "../Logger.hpp"
#include "../Rustify.hpp"
#include "Help.hpp"

#define HANDLE_GLOBAL_OPTS(HELP_ARGS)             \
  if (arg == "-h" || arg == "--help") {           \
    return helpMain(HELP_ARGS);                   \
  } else if (arg == "-v" || arg == "--verbose") { \
    Logger::setLevel(LogLevel::debug);            \
  } else if (arg == "-q" || arg == "--quiet") {   \
    Logger::setLevel(LogLevel::off);              \
  }

// long, short, description
static inline const Vec<Tuple<StringRef, StringRef, StringRef>>
    GLOBAL_OPT_HELPS{
        {"--verbose", "-v", "Use verbose output"},
        {"--quiet", "-q", "Do not print poac log messages"},
        {"--help", "-h", "Print help"},
    };

void printHeader(StringRef header) noexcept;
void printUsage(StringRef cmd, StringRef usage) noexcept;
void printOption(
    StringRef lng, StringRef shrt, StringRef desc, StringRef placeholder = ""
) noexcept;
void printCommand(StringRef name, StringRef desc) noexcept;
void printGlobalOpts() noexcept;
