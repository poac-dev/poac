#pragma once

#include "Rustify.hpp"
#include "TermColor.hpp"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <type_traits>
#include <utility>

namespace logger {

enum class Level : u8 {
  Off = 0, // --quiet
  Error = 1,
  Warn = 2,
  Info = 3, // default
  Debug = 4 // --verbose
};

namespace detail {

  class Logger {
    static constexpr int INFO_OFFSET = 12;
    Level level = Level::Info;

    Logger() noexcept = default;

  public:
    // Logger is a singleton
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept = delete;
    Logger& operator=(Logger&&) noexcept = delete;
    ~Logger() noexcept = default;

    static Logger& instance() noexcept;
    static void setLevel(Level level) noexcept;
    static Level getLevel() noexcept;

    template <typename... Ts>
    static void error(Ts&&... msgs) noexcept {
      logln(Level::Error, std::forward<Ts>(msgs)...);
    }
    template <typename... Ts>
    static void warn(Ts&&... msgs) noexcept {
      logln(Level::Warn, std::forward<Ts>(msgs)...);
    }
    template <typename... Ts>
    static void info(Ts&&... msgs) noexcept {
      logln(Level::Info, std::forward<Ts>(msgs)...);
    }
    template <typename... Ts>
    static void debug(Ts&&... msgs) noexcept {
      logln(Level::Debug, std::forward<Ts>(msgs)...);
    }

  private:
    template <typename T, typename... Ts>
      requires(((Writer<T> || Display<T>) && Display<Ts>) && ...)
    static void logln(Level level, T&& val, Ts&&... msgs) noexcept {
      if constexpr (Writer<T>) {
        loglnImpl(std::forward<T>(val), level, std::forward<Ts>(msgs)...);
      } else {
        loglnImpl(
            std::cerr, level, std::forward<T>(val), std::forward<Ts>(msgs)...
        );
      }
    }

    template <typename... Ts>
      requires(Display<Ts> && ...)
    static void
    loglnImpl(std::ostream& os, Level level, Ts&&... msgs) noexcept {
      instance().log(os, level, std::forward<Ts>(msgs)..., '\n');
    }

    template <typename T, typename... Ts>
      requires(Display<T> && (Display<Ts> && ...))
    void log(std::ostream& os, Level level, T&& head, Ts&&... msgs) noexcept {
      // For other than `info`, header means just the first argument.

      if (level <= this->level) {
        switch (level) {
          case Level::Off:
            return;
          case Level::Error:
            os << bold(red("Error: ")) << std::forward<T>(head);
            break;
          case Level::Warn:
            os << bold(yellow("Warning: ")) << std::forward<T>(head);
            break;
          case Level::Info:
            os << std::right;
            if (shouldColor()) {
              // Color escape sequences are not visible but affect std::setw.
              constexpr int COLOR_ESCAPE_SEQ_LEN = 9;
              os << std::setw(INFO_OFFSET + COLOR_ESCAPE_SEQ_LEN);
            } else {
              os << std::setw(INFO_OFFSET);
            }
            os << bold(green(std::forward<T>(head))) << ' ';
            break;
          case Level::Debug:
            os << gray("[") << "Poac " << blue("DEBUG") << ' '
               << std::forward<T>(head) << gray("] ");
            break;
        }
        (os << ... << std::forward<Ts>(msgs)) << std::flush;
      }
    }
  };

} // namespace detail

template <typename... Ts>
void
error(Ts&&... msgs) noexcept {
  detail::Logger::error(std::forward<Ts>(msgs)...);
}
template <typename... Ts>
void
warn(Ts&&... msgs) noexcept {
  detail::Logger::warn(std::forward<Ts>(msgs)...);
}
template <typename... Ts>
void
info(Ts&&... msgs) noexcept {
  detail::Logger::info(std::forward<Ts>(msgs)...);
}

template <typename... Ts>
struct debug { // NOLINT(readability-identifier-naming)
  explicit debug(
      Ts&&... msgs, const source_location& loc = source_location::current()
  ) noexcept {
    detail::Logger::debug(loc.function_name(), std::forward<Ts>(msgs)...);
  }
};
template <typename... Ts>
debug(Ts&&...) -> debug<Ts...>;

void setLevel(Level level) noexcept;
Level getLevel() noexcept;

} // namespace logger

bool isVerbose() noexcept;
bool isQuiet() noexcept;
