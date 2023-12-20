#include "Logger.hpp"

#include "Rustify.hpp"

#include <iostream>

Logger& Logger::getInstance() noexcept {
  static Logger instance;
  return instance;
}

void Logger::setLevel(LogLevel level) noexcept {
  getInstance().level = level;
}

LogLevel Logger::getLevel() noexcept {
  return getInstance().level;
}
