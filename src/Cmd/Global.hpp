// Global options and helper functions for commands.

#pragma once

#include "../Logger.hpp"
#include "../Rustify.hpp"
#include "Help.hpp"

#include <tuple>

#define HANDLE_GLOBAL_OPTS(HELP_ARGS)             \
  if (arg == "-h" || arg == "--help") {           \
    return helpMain(HELP_ARGS);                   \
  } else if (arg == "-v" || arg == "--verbose") { \
    Logger::setLevel(LogLevel::debug);            \
  } else if (arg == "-q" || arg == "--quiet") {   \
    Logger::setLevel(LogLevel::off);              \
  }

// long, short, description
static inline constexpr Arr<Tuple<StringRef, StringRef, StringRef>, 3>
    GLOBAL_OPT_HELPS{
        std::make_tuple("--verbose", "-v", "Use verbose output"),
        {"--quiet", "-q", "Do not print poac log messages"},
        {"--help", "-h", "Print help"},
    };

void printHeader(StringRef) noexcept;
void printUsage(StringRef, StringRef) noexcept;
void printOption(StringRef, StringRef, StringRef, StringRef = "") noexcept;
void printCommand(StringRef, StringRef) noexcept;
void printGlobalOpts() noexcept;

bool commandExists(const String&) noexcept;
