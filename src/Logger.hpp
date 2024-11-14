#pragma once

#include "Rustify/Traits.hpp"
#include "TermColor.hpp"

#include <cstdint>
#include <fmt/core.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

namespace logger {

enum class Level : uint8_t {
  Off = 0, // --quiet, -q
  Error = 1,
  Warn = 2,
  Info = 3, // default
  Debug = 4, // --verbose, -v
  Trace = 5, // -vv
};

template <typename T>
concept MaybeWriter = Writer<T> || Display<T>;

template <typename Fn>
concept HeadProcessor =
    std::is_nothrow_invocable_v<Fn, std::string_view> && Display<
        std::invoke_result_t<Fn, std::string_view>>;

class Logger {
  Level level = Level::Info;

  constexpr Logger() noexcept = default;

public:
  // Logger is a singleton
  constexpr Logger(const Logger&) = delete;
  constexpr Logger& operator=(const Logger&) = delete;
  constexpr Logger(Logger&&) noexcept = delete;
  constexpr Logger& operator=(Logger&&) noexcept = delete;
  constexpr ~Logger() noexcept = default;

  static Logger& instance() noexcept {
    static Logger instance;
    return instance;
  }
  static void setLevel(Level level) noexcept {
    instance().level = level;
  }
  static Level getLevel() noexcept {
    return instance().level;
  }

  static void error(MaybeWriter auto&&... msgs) noexcept {
    logln(
        Level::Error,
        [](const std::string_view head) noexcept { return bold(red(head)); },
        "Error: ", std::forward<decltype(msgs)>(msgs)...
    );
  }
  static void warn(MaybeWriter auto&&... msgs) noexcept {
    logln(
        Level::Warn,
        [](const std::string_view head) noexcept { return bold(yellow(head)); },
        "Warning: ", std::forward<decltype(msgs)>(msgs)...
    );
  }
  static void info(MaybeWriter auto&&... msgs) noexcept {
    logln(
        Level::Info,
        [](const std::string_view head) noexcept {
          const std::string_view fmtStr = shouldColor() ? "{:>21} " : "{:>12} ";
          return fmt::format(fmt::runtime(fmtStr), bold(green(head)));
        },
        std::forward<decltype(msgs)>(msgs)...
    );
  }
  static void debug(MaybeWriter auto&&... msgs) noexcept {
    debuglike(
        Level::Debug, blue("DEBUG"), std::forward<decltype(msgs)>(msgs)...
    );
  }
  static void trace(MaybeWriter auto&&... msgs) noexcept {
    debuglike(
        Level::Trace, cyan("TRACE"), std::forward<decltype(msgs)>(msgs)...
    );
  }

private:
  static void debuglike(
      Level level, const std::string_view lvlStr, const std::string_view func,
      Writer auto&& writer, Display auto&&... msgs
  ) noexcept {
    // Swap func and writer, since for logln, writer should appear first for
    // the msgs parameter pack.  In this case, func will be recognized as
    // head.
    debuglike(
        level, lvlStr, writer, func, std::forward<decltype(msgs)>(msgs)...
    );
  }
  static void debuglike(
      Level level, const std::string_view lvlStr,
      MaybeWriter auto&& maybeWriter, Display auto&&... msgs
  ) noexcept {
    logln(
        level,
        [lvlStr](const std::string_view func) noexcept {
          return fmt::format(
              "{}Poac {} {}{} ", gray("["), lvlStr, func, gray("]")
          );
        },
        std::forward<decltype(maybeWriter)>(maybeWriter),
        std::forward<decltype(msgs)>(msgs)...
    );
  }

  static void logln(
      Level level, HeadProcessor auto&& processHead, Writer auto&& writer,
      Display auto&&... msgs
  ) noexcept {
    loglnImpl(
        std::forward<decltype(writer)>(writer), level,
        std::forward<decltype(processHead)>(processHead),
        std::forward<decltype(msgs)>(msgs)...
    );
  }
  static void logln(
      Level level, HeadProcessor auto&& processHead, Display auto&&... msgs
  ) noexcept {
    loglnImpl(
        std::cerr, level, std::forward<decltype(processHead)>(processHead),
        std::forward<decltype(msgs)>(msgs)...
    );
  }

  static void loglnImpl(
      std::ostream& os, Level level, HeadProcessor auto&& processHead,
      Display auto&&... msgs
  ) noexcept {
    instance().log(
        os, level, std::forward<decltype(processHead)>(processHead),
        std::forward<decltype(msgs)>(msgs)..., '\n'
    );
  }

  void
  log(std::ostream& os, Level level, HeadProcessor auto&& processHead,
      Display auto&& head, Display auto&&... msgs) noexcept {
    if (level <= this->level) {
      os << std::invoke(
          std::forward<decltype(processHead)>(processHead),
          std::forward<decltype(head)>(head)
      );
      (os << ... << std::forward<decltype(msgs)>(msgs)) << std::flush;
    }
  }
};

inline void
error(MaybeWriter auto&&... msgs) noexcept {
  Logger::error(std::forward<decltype(msgs)>(msgs)...);
}
inline void
warn(MaybeWriter auto&&... msgs) noexcept {
  Logger::warn(std::forward<decltype(msgs)>(msgs)...);
}
inline void
info(MaybeWriter auto&&... msgs) noexcept {
  Logger::info(std::forward<decltype(msgs)>(msgs)...);
}

template <MaybeWriter... Ts>
struct debug { // NOLINT(readability-identifier-naming)
  explicit debug(
      Ts&&... msgs,
      const std::source_location& loc = std::source_location::current()
  ) noexcept {
    Logger::debug(loc.function_name(), std::forward<Ts>(msgs)...);
  }
};
template <MaybeWriter... Ts>
debug(Ts&&...) -> debug<Ts...>;

template <MaybeWriter... Ts>
struct trace { // NOLINT(readability-identifier-naming)
  explicit trace(
      Ts&&... msgs,
      const std::source_location& loc = std::source_location::current()
  ) noexcept {
    Logger::trace(loc.function_name(), std::forward<Ts>(msgs)...);
  }
};
template <MaybeWriter... Ts>
trace(Ts&&...) -> trace<Ts...>;

inline void
setLevel(Level level) noexcept {
  Logger::setLevel(level);
}
inline Level
getLevel() noexcept {
  return Logger::getLevel();
}

} // namespace logger

inline bool
isVerbose() noexcept {
  return logger::getLevel() >= logger::Level::Debug;
}
inline bool
isQuiet() noexcept {
  return logger::getLevel() == logger::Level::Off;
}
