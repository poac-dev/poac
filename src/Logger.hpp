#pragma once

#include "Rustify.hpp"

enum class LogLevel : u8 { ERROR = 0, WARNING = 1, INFO = 2, DEBUG = 3 };

class Logger {
public:
  static Logger& getInstance();
  static void setLevel(LogLevel level);
  static void error(StringRef message);
  static void warning(StringRef message);
  static void info(StringRef message);
  static void debug(StringRef message);
  void log(LogLevel messageLevel, StringRef message);

private:
  LogLevel level = LogLevel::WARNING;

  Logger() {}

  // Delete copy constructor and assignment operator to prevent copying
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};
