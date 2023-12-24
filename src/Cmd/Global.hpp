// Global options and helper functions for commands.

#pragma once

#include "../Logger.hpp"
#include "../Rustify.hpp"
#include "../TermColor.hpp"
#include "Help.hpp"

#include <cstdlib>
#include <tuple>

#define HANDLE_GLOBAL_OPTS(HELP_ARGS)                  \
  if (arg == "-h" || arg == "--help") {                \
    return helpMain(HELP_ARGS);                        \
  } else if (arg == "-v" || arg == "--verbose") {      \
    Logger::setLevel(LogLevel::debug);                 \
  } else if (arg == "-q" || arg == "--quiet") {        \
    Logger::setLevel(LogLevel::off);                   \
  } else if (arg == "--color") {                       \
    if (i + 1 < args.size()) {                         \
      setColorMode(args[++i]);                         \
    } else {                                           \
      Logger::error("missing argument for `--color`"); \
      return EXIT_FAILURE;                             \
    }                                                  \
  }

// short, long, placeholder, description
static inline constexpr Arr<
    Tuple<StringRef, StringRef, StringRef, StringRef>, 4>
    GLOBAL_OPT_HELPS{
        std::make_tuple("-v", "--verbose", "", "Use verbose output"),
        {"-q", "--quiet", "", "Do not print poac log messages"},
        {"", "--color", "<WHEN>", "Coloring: auto, always, never"},
        {"-h", "--help", "", "Print help"},
    };

void printHeader(StringRef) noexcept;
void printUsage(StringRef, StringRef) noexcept;
void printOption(StringRef, StringRef, StringRef, StringRef = "") noexcept;
void printCommand(StringRef, StringRef) noexcept;
void printGlobalOpts() noexcept;

bool commandExists(StringRef) noexcept;
