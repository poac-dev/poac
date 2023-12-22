#include "Logger.hpp"

Logger& Logger::instance() noexcept {
  static Logger instance;
  return instance;
}

void Logger::setLevel(LogLevel level) noexcept {
  instance().level = level;
}

LogLevel Logger::getLevel() noexcept {
  return instance().level;
}

bool isVerbose() noexcept {
  return Logger::getLevel() == LogLevel::debug;
}

bool isQuiet() noexcept {
  return Logger::getLevel() == LogLevel::off;
}
