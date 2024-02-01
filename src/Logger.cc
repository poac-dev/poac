#include "Logger.hpp"

Logger&
Logger::instance() noexcept {
  static Logger INSTANCE;
  return INSTANCE;
}

void
Logger::setLevel(LogLevel level) noexcept {
  instance().level = level;
}

LogLevel
Logger::getLevel() noexcept {
  return instance().level;
}

bool
isVerbose() noexcept {
  return Logger::getLevel() == LogLevel::Debug;
}

bool
isQuiet() noexcept {
  return Logger::getLevel() == LogLevel::Off;
}
