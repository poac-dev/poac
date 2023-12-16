#include "Logger.hpp"

#include "Rustify.hpp"

#include <iostream>

Logger& Logger::getInstance() {
  static Logger instance;
  return instance;
}

void Logger::setLevel(LogLevel level) { getInstance().level = level; }

void Logger::error(StringRef message) {
  getInstance().log(LogLevel::ERROR, message);
}

void Logger::warning(StringRef message) {
  getInstance().log(LogLevel::WARNING, message);
}

void Logger::info(StringRef message) {
  getInstance().log(LogLevel::INFO, message);
}

void Logger::debug(StringRef message) {
  getInstance().log(LogLevel::DEBUG, message);
}

void Logger::log(LogLevel messageLevel, StringRef message) {
  if (messageLevel <= level) {
    switch (messageLevel) {
      case LogLevel::ERROR:
        std::cerr << "[ERROR] " << message << '\n';
        break;
      case LogLevel::WARNING:
        std::cout << "[WARN] " << message << '\n';
        break;
      case LogLevel::INFO:
        std::cout << "[INFO] " << message << '\n';
        break;
      case LogLevel::DEBUG:
        std::cout << "[DEBUG] " << message << '\n';
        break;
    }
  }
}
