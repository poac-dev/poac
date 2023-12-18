#pragma once

#include "Rustify.hpp"
#include "TermColor.hpp"

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
  static void setLevel(LogLevel level);

  template <typename... Args>
  static void error(Args... message) {
    getInstance().log(LogLevel::error, std::forward<Args>(message)...);
  }
  template <typename... Args>
  static void warn(Args... message) {
    getInstance().log(LogLevel::warning, std::forward<Args>(message)...);
  }
  template <typename... Args>
  static void status(StringRef header, Args... message) {
    status(header, std::forward<Args>(message)...);
  }
  template <typename... Args>
  static void status(Args... message) {
    getInstance().log(LogLevel::status, std::forward<Args>(message)...);
  }
  template <typename... Args>
  static void debug(Args... message) {
    getInstance().log(LogLevel::debug, std::forward<Args>(message)...);
  }

  template <typename... Args>
  void log(LogLevel messageLevel, Args... message) {
    if (messageLevel <= level) {
      switch (messageLevel) {
        case LogLevel::error:
          std::cerr << bold(red("Error: "));
          (std::cerr << ... << message) << '\n';
          break;
        case LogLevel::warning:
          std::cout << bold(yellow("Warning: "));
          (std::cout << ... << message) << '\n';
          break;
        case LogLevel::status:
          (std::cout << ... << message) << '\n';
          break;
        case LogLevel::debug:
          std::cout << "[Poac] ";
          (std::cout << ... << message) << '\n';
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
