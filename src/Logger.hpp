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
    getInstance().log(LogLevel::error, std::forward<Args>(message)...);
  }
  template <typename... Args>
  static void warn(Args&&... message) {
    getInstance().log(LogLevel::warning, std::forward<Args>(message)...);
  }
  template <typename T, typename... Args>
  static void status(T&& header, Args&&... message) {
    getInstance().log(
        LogLevel::status, std::forward<T>(header),
        std::forward<Args>(message)...
    );
  }
  template <typename... Args>
  static void debug(Args&&... message) {
    getInstance().log(LogLevel::debug, std::forward<Args>(message)...);
  }

  template <typename T, typename... Args>
  void log(LogLevel messageLevel, T&& header, Args&&... message) {
    // For other than `status`, header means just the first argument.  For
    // `status`, header means its header.

    if (messageLevel <= level) {
      switch (messageLevel) {
        case LogLevel::error:
          std::cerr << bold(red("Error: ")) << std::forward<T>(header);
          (std::cerr << ... << std::forward<Args>(message)) << '\n';
          break;
        case LogLevel::warning:
          std::cout << bold(yellow("Warning: ")) << std::forward<T>(header);
          (std::cout << ... << std::forward<Args>(message)) << '\n';
          break;
        case LogLevel::status:
          if (should_color()) {
            std::cout << std::setw(27) << bold(green(std::forward<T>(header)))
                      << ' ';
          } else {
            std::cout << std::setw(12) << std::forward<T>(header) << ' ';
          }
          (std::cout << ... << std::forward<Args>(message)) << '\n';
          break;
        case LogLevel::debug:
          std::cout << "[Poac] " << std::forward<T>(header);
          (std::cout << ... << std::forward<Args>(message)) << '\n';
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
