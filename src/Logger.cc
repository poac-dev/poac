#include "Logger.hpp"

#include "Rustify.hpp"

#include <iostream>

Logger& Logger::getInstance() {
  static Logger instance;
  return instance;
}

void Logger::setLevel(LogLevel level) {
  getInstance().level = level;
}
