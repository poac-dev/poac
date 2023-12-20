#include "Logger.hpp"

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

bool isVerbose() noexcept {
  return Logger::getLevel() == LogLevel::debug;
}

bool isQuiet() noexcept {
  return Logger::getLevel() == LogLevel::off;
}
