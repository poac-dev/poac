#pragma once

#include "../Logger.hpp"
#include "../Rustify.hpp"
#include "../TermColor.hpp"
#include "Help.hpp"

#include <iostream>

#define HANDLE_GLOBAL_OPTS(HELP_ARGS)           \
  if (arg == "-h" || arg == "--help") {         \
    return helpMain(HELP_ARGS);                 \
  } else if (arg == "--verbose") {              \
    Logger::setLevel(LogLevel::debug);          \
  } else if (arg == "-q" || arg == "--quiet") { \
    Logger::setLevel(LogLevel::off);            \
  }

static inline const HashMap<StringRef, StringRef> GLOBAL_OPT_HELPS = {
    {"--verbose", "Use verbose output"},
    {"-q, --quiet", "Do not print poac log messages"},
    {"-h, --help", "Print help"},
};

inline void printHeader(StringRef header) noexcept {
  std::cout << bold(green(header)) << '\n';
}

inline void printUsage(StringRef usage) noexcept {
  std::cout << bold(green("Usage: ")) << usage << '\n';
}

inline void printOption(StringRef name, StringRef desc) noexcept {
  std::cout << "  " << std::left << std::setw(25) << name << desc << '\n';
}

inline void printCommand(StringRef name, StringRef desc) noexcept {
  std::cout << "  " << std::left << std::setw(10) << name << desc << '\n';
}

inline void printGlobalOpts() noexcept {
  for (const auto& [name, desc] : GLOBAL_OPT_HELPS) {
    printOption(name, desc);
  }
}
