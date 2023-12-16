#pragma once

#include "Rustify.hpp"

#include <iostream>
#include <utility>

enum class LogLevel : u8 { ERROR = 0, WARNING = 1, INFO = 2, DEBUG = 3 };

class Logger {
public:
  static Logger& getInstance();
  static void setLevel(LogLevel level);

  template <typename... Args>
  static void error(Args... message) {
    getInstance().log(LogLevel::ERROR, std::forward<Args>(message)...);
  }
  template <typename... Args>
  static void warning(Args... message) {
    getInstance().log(LogLevel::WARNING, std::forward<Args>(message)...);
  }
  template <typename... Args>
  static void info(Args... message) {
    getInstance().log(LogLevel::INFO, std::forward<Args>(message)...);
  }
  template <typename... Args>
  static void debug(Args... message) {
    getInstance().log(LogLevel::DEBUG, std::forward<Args>(message)...);
  }

  template <typename... Args>
  void log(LogLevel messageLevel, Args... message) {
    if (messageLevel <= level) {
      switch (messageLevel) {
        case LogLevel::ERROR:
          std::cerr << "[ERROR] ";
          (std::cerr << ... << message) << '\n';
          break;
        case LogLevel::WARNING:
          std::cout << "[WARNING] ";
          (std::cout << ... << message) << '\n';
          break;
        case LogLevel::INFO:
          std::cout << "[INFO] ";
          (std::cout << ... << message) << '\n';
          break;
        case LogLevel::DEBUG:
          std::cout << "[DEBUG] ";
          (std::cout << ... << message) << '\n';
          break;
      }
    }
  }

private:
  LogLevel level = LogLevel::WARNING;

  Logger() {}

  // Delete copy constructor and assignment operator to prevent copying
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};
