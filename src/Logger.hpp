#pragma once

#include "Rustify.hpp"
#include "TermColor.hpp"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <type_traits>
#include <utility>

enum class LogLevel : u8 {
  Off = 0, // --quiet
  Error = 1,
  Warning = 2,
  Info = 3, // default
  Debug = 4 // --verbose
};

class Logger {
  static constexpr int INFO_OFFSET = 12;
  LogLevel level = LogLevel::Info;

  Logger() noexcept = default;

public:
  // Logger is a singleton
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) noexcept = delete;
  Logger& operator=(Logger&&) noexcept = delete;
  ~Logger() noexcept = default;

  static Logger& instance() noexcept;
  static void setLevel(LogLevel level) noexcept;
  static LogLevel getLevel() noexcept;

  template <typename... Ts>
  static void error(Ts&&... msgs) noexcept {
    logln(LogLevel::Error, std::forward<Ts>(msgs)...);
  }
  template <typename... Ts>
  static void warn(Ts&&... msgs) noexcept {
    logln(LogLevel::Warning, std::forward<Ts>(msgs)...);
  }
  template <typename... Ts>
  static void info(Ts&&... msgs) noexcept {
    logln(LogLevel::Info, std::forward<Ts>(msgs)...);
  }
  template <typename... Ts>
  static void debug(Ts&&... msgs) noexcept {
    logln(LogLevel::Debug, std::forward<Ts>(msgs)...);
  }

private:
  template <typename T, typename... Ts>
    requires(((Writer<T> || Display<T>) && Display<Ts>) && ...)
  static void logln(LogLevel level, T&& val, Ts&&... msgs) noexcept {
    if constexpr (Writer<T>) {
      loglnImpl(std::forward<T>(val), level, std::forward<Ts>(msgs)...);
    } else {
      loglnImpl(
          std::cerr, level, std::forward<T>(val), std::forward<Ts>(msgs)...
      );
    }
  }

  template <typename... Ts>
    requires(Display<Ts> && ...)
  static void
  loglnImpl(std::ostream& os, LogLevel level, Ts&&... msgs) noexcept {
    instance().log(os, level, std::forward<Ts>(msgs)..., '\n');
  }

  template <typename T, typename... Ts>
    requires(Display<T> && (Display<Ts> && ...))
  void
  log(std::ostream& os, LogLevel level, T&& header, Ts&&... msgs) noexcept {
    // For other than `info`, header means just the first argument.

    if (level <= this->level) {
      switch (level) {
        case LogLevel::Off:
          return;
        case LogLevel::Error:
          os << bold(red("Error: ")) << std::forward<T>(header);
          break;
        case LogLevel::Warning:
          os << bold(yellow("Warning: ")) << std::forward<T>(header);
          break;
        case LogLevel::Info:
          os << std::right;
          if (shouldColor()) {
            // Color escape sequences are not visible but affect std::setw.
            constexpr int COLOR_ESCAPE_SEQ_LEN = 9;
            os << std::setw(INFO_OFFSET + COLOR_ESCAPE_SEQ_LEN);
          } else {
            os << std::setw(INFO_OFFSET);
          }
          os << bold(green(std::forward<T>(header))) << ' ';
          break;
        case LogLevel::Debug:
          os << "[Poac] " << std::forward<T>(header);
          break;
      }
      (os << ... << std::forward<Ts>(msgs)) << std::flush;
    }
  }
};

bool isVerbose() noexcept;
bool isQuiet() noexcept;
