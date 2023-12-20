#pragma once

#include "Rustify.hpp"
#include "TermColor.hpp"

#include <iomanip>
#include <iostream>
#include <utility>

enum class LogLevel : u8 {
  off = 0, // --quiet
  error = 1,
  warning = 2,
  status = 3, // default
  debug = 4 // --verbose
};

class Logger {
public:
  static Logger& getInstance() noexcept;
  static void setLevel(LogLevel) noexcept;
  static LogLevel getLevel() noexcept;

  template <typename... Args>
  static void error(Args&&... message) noexcept {
    logln(std::cerr, LogLevel::error, std::forward<Args>(message)...);
  }
  template <typename... Args>
  static void warn(Args&&... message) noexcept {
    logln(std::cout, LogLevel::warning, std::forward<Args>(message)...);
  }
  template <typename T, typename... Args>
  static void status(T&& header, Args&&... message) noexcept {
    logln(
        std::cout, LogLevel::status, std::forward<T>(header),
        std::forward<Args>(message)...
    );
  }
  template <typename... Args>
  static void debug(Args&&... message) noexcept {
    logln(std::cout, LogLevel::debug, std::forward<Args>(message)...);
  }

  template <typename T, typename... Args>
  static void logln(
      std::ostream& os, LogLevel messageLevel, T&& header, Args&&... message
  ) noexcept {
    log(os, messageLevel, std::forward<T>(header),
        std::forward<Args>(message)..., '\n');
  }
  template <typename T, typename... Args>
  static void
  log(std::ostream& os, LogLevel messageLevel, T&& header,
      Args&&... message) noexcept {
    getInstance().logImpl(
        os, messageLevel, std::forward<T>(header),
        std::forward<Args>(message)...
    );
  }

  template <typename T, typename... Args>
  void logImpl(
      std::ostream& os, LogLevel messageLevel, T&& header, Args&&... message
  ) noexcept {
    // For other than `status`, header means just the first argument.  For
    // `status`, header means its header.

    if (messageLevel <= level) {
      switch (messageLevel) {
        case LogLevel::off:
          break;
        case LogLevel::error:
          os << bold(red("Error: ")) << std::forward<T>(header);
          (os << ... << std::forward<Args>(message));
          break;
        case LogLevel::warning:
          os << bold(yellow("Warning: ")) << std::forward<T>(header);
          (os << ... << std::forward<Args>(message));
          break;
        case LogLevel::status:
          if (shouldColor()) {
            os << std::right << std::setw(27)
               << bold(green(std::forward<T>(header))) << ' ';
          } else {
            os << std::right << std::setw(12) << std::forward<T>(header) << ' ';
          }
          (os << ... << std::forward<Args>(message));
          break;
        case LogLevel::debug:
          os << "[Poac] " << std::forward<T>(header);
          (os << ... << std::forward<Args>(message));
          break;
      }
    }
  }

private:
  LogLevel level = LogLevel::status;

  Logger() noexcept {}

  // Delete copy constructor and assignment operator to prevent copying
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};

bool isVerbose() noexcept;
bool isQuiet() noexcept;
