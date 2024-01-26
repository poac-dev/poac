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
  LogLevel level = LogLevel::info;
  static constexpr int INFO_OFFSET = 12;

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
  template <typename Sink, typename... Args>
    requires(
        ((std::is_base_of_v<std::ostream, std::remove_reference_t<Sink>>
          || Display<Sink>)
         && Display<Args>)
        && ...
    )
  static void logln(LogLevel level, Sink&& sink, Args&&... msgs) noexcept {
    if constexpr (std::is_base_of_v<
                      std::ostream, std::remove_reference_t<Sink>>) {
      loglnImpl(std::forward<Sink>(sink), level, std::forward<Args>(msgs)...);
    } else {
      loglnImpl(
          std::cerr, level, std::forward<Sink>(sink),
          std::forward<Args>(msgs)...
      );
    }
  }

  template <typename T, typename... Args>
    requires(Display<T> && (Display<Args> && ...))
  static void loglnImpl(
      std::ostream& os, LogLevel messageLevel, T&& header, Args&&... message
  ) noexcept {
    instance().log(
        os, messageLevel, std::forward<T>(header),
        std::forward<Args>(message)..., '\n'
    );
  }

  template <typename T, typename... Args>
    requires(Display<T> && (Display<Args> && ...))
  void
  log(std::ostream& os, LogLevel messageLevel, T&& header,
      Args&&... message) noexcept {
    // For other than `info`, header means just the first argument.  For
    // `info`, header means its header.

    if (messageLevel <= level) {
      switch (messageLevel) {
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
      (os << ... << std::forward<Args>(message)) << std::flush;
    }
  }
};

bool isVerbose() noexcept;
bool isQuiet() noexcept;
