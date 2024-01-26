#pragma once

#include "Rustify.hpp"
#include "TermColor.hpp"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <type_traits>
#include <utility>

enum class LogLevel : u8 {
  off = 0, // --quiet
  error = 1,
  warning = 2,
  info = 3, // default
  debug = 4 // --verbose
};

class Logger {
  static constexpr int INFO_OFFSET = 12;
  LogLevel level = LogLevel::info;

  // Logger is a singleton
  Logger() noexcept = default;

public:
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) noexcept = delete;
  Logger& operator=(Logger&&) noexcept = delete;
  ~Logger() noexcept = default;

  static Logger& instance() noexcept;
  static void setLevel(LogLevel level) noexcept;
  static LogLevel getLevel() noexcept;

  template <typename... Args>
  static void error(Args&&... msgs) noexcept {
    logln(LogLevel::error, std::forward<Args>(msgs)...);
  }
  template <typename... Args>
  static void warn(Args&&... msgs) noexcept {
    logln(LogLevel::warning, std::forward<Args>(msgs)...);
  }
  template <typename... Args>
  static void info(Args&&... msgs) noexcept {
    logln(LogLevel::info, std::forward<Args>(msgs)...);
  }
  template <typename... Args>
  static void debug(Args&&... msgs) noexcept {
    logln(LogLevel::debug, std::forward<Args>(msgs)...);
  }

private:
  template <typename T, typename... Args>
    requires(((Writer<T> || Display<T>) && Display<Args>) && ...)
  static void logln(LogLevel level, T&& val, Args&&... msgs) noexcept {
    if constexpr (Writer<T>) {
      loglnImpl(std::forward<T>(val), level, std::forward<Args>(msgs)...);
    } else {
      loglnImpl(
          std::cerr, level, std::forward<T>(val), std::forward<Args>(msgs)...
      );
    }
  }

  template <typename... Args>
    requires(Display<Args> && ...)
  static void
  loglnImpl(std::ostream& os, LogLevel level, Args&&... msgs) noexcept {
    instance().log(os, level, std::forward<Args>(msgs)..., '\n');
  }

  template <typename T, typename... Args>
    requires(Display<T> && (Display<Args> && ...))
  void
  log(std::ostream& os, LogLevel level, T&& header, Args&&... msgs) noexcept {
    // For other than `info`, header means just the first argument.

    if (level <= this->level) {
      switch (level) {
        case LogLevel::off:
          return;
        case LogLevel::error:
          os << bold(red("Error: ")) << std::forward<T>(header);
          break;
        case LogLevel::warning:
          os << bold(yellow("Warning: ")) << std::forward<T>(header);
          break;
        case LogLevel::info:
          os << std::right;
          if (shouldColor()) {
            // Color escape sequences are not visible but affect std::setw.
            constexpr int COLOR_ESCAPE_SEQ_LEN = 15;
            os << std::setw(INFO_OFFSET + COLOR_ESCAPE_SEQ_LEN);
          } else {
            os << std::setw(INFO_OFFSET);
          }
          os << bold(green(std::forward<T>(header))) << ' ';
          break;
        case LogLevel::debug:
          os << "[Poac] " << std::forward<T>(header);
          break;
      }
      (os << ... << std::forward<Args>(msgs)) << std::flush;
    }
  }
};

bool isVerbose() noexcept;
bool isQuiet() noexcept;
