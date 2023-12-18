#pragma once

#include "Rustify.hpp"
#include "TermColor.hpp"

#include <iomanip>
#include <iostream>
#include <utility>

enum class LogLevel : u8 {
  error = 0,
  warning = 1,
  status = 2, // default
  debug = 3 // --verbose
};

class Logger {
public:
  static Logger& getInstance();
  static void setLevel(LogLevel);

  template <typename... Args>
  static void error(Args&&... message) {
    getInstance().logln(
        std::cerr, LogLevel::error, std::forward<Args>(message)...
    );
  }
  template <typename... Args>
  static void warn(Args&&... message) {
    getInstance().logln(
        std::cout, LogLevel::warning, std::forward<Args>(message)...
    );
  }
  template <typename T, typename... Args>
  static void status(T&& header, Args&&... message) {
    getInstance().logln(
        std::cout, LogLevel::status, std::forward<T>(header),
        std::forward<Args>(message)...
    );
  }
  template <typename... Args>
  static void debug(Args&&... message) {
    getInstance().logln(
        std::cout, LogLevel::debug, std::forward<Args>(message)...
    );
  }

  template <typename T, typename... Args>
  void logln(
      std::ostream& os, LogLevel messageLevel, T&& header, Args&&... message
  ) {
    log(os, messageLevel, std::forward<T>(header),
        std::forward<Args>(message)..., '\n');
  }

  template <typename T, typename... Args>
  void
  log(std::ostream& os, LogLevel messageLevel, T&& header, Args&&... message) {
    // For other than `status`, header means just the first argument.  For
    // `status`, header means its header.

    if (messageLevel <= level) {
      switch (messageLevel) {
        case LogLevel::error:
          os << bold(red("Error: ")) << std::forward<T>(header);
          (os << ... << std::forward<Args>(message));
          break;
        case LogLevel::warning:
          os << bold(yellow("Warning: ")) << std::forward<T>(header);
          (os << ... << std::forward<Args>(message));
          break;
        case LogLevel::status:
          if (should_color()) {
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

  Logger() {}

  // Delete copy constructor and assignment operator to prevent copying
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};
