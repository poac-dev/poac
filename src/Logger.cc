#include "Logger.hpp"

namespace logger {

namespace detail {

  Logger& Logger::instance() noexcept {
    static Logger instance;
    return instance;
  }

  void Logger::setLevel(Level level) noexcept {
    instance().level = level;
  }

  Level Logger::getLevel() noexcept {
    return instance().level;
  }

} // namespace detail

void
setLevel(Level level) noexcept {
  detail::Logger::setLevel(level);
}

Level
getLevel() noexcept {
  return detail::Logger::getLevel();
}

} // namespace logger

bool
isVerbose() noexcept {
  return logger::getLevel() == logger::Level::Debug;
}

bool
isQuiet() noexcept {
  return logger::getLevel() == logger::Level::Off;
}
