#pragma once

#include "Rustify.hpp"
#include "TermColor.hpp"

#include <fmt/core.h>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <type_traits>
#include <utility>

namespace logger {

enum class Level : u8 {
  Off = 0, // --quiet, -q
  Error = 1,
  Warn = 2,
  Info = 3, // default
  Debug = 4, // --verbose, -v
  Trace = 5, // -vv
};

template <typename T>
concept MaybeWriter = Writer<T> || Display<T>;

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

  static inline Logger& instance() noexcept {
    static Logger instance;
    return instance;
  }
  static inline void setLevel(Level level) noexcept {
    instance().level = level;
  }
  static inline Level getLevel() noexcept {
    return instance().level;
  }

  static void error(MaybeWriter auto&&... msgs) noexcept {
    logln(
        Level::Error,
        [](const StringRef head) noexcept { return bold(red(head)); },
        "Error: ", std::forward<decltype(msgs)>(msgs)...
    );
  }
  static void warn(MaybeWriter auto&&... msgs) noexcept {
    logln(
        Level::Warn,
        [](const StringRef head) noexcept { return bold(yellow(head)); },
        "Warning: ", std::forward<decltype(msgs)>(msgs)...
    );
  }
  static void info(MaybeWriter auto&&... msgs) noexcept {
    logln(
        Level::Info,
        [](const StringRef head) noexcept -> String {
          const StringRef fmtStr = shouldColor() ? "{:>21} " : "{:>12} ";
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
      Level level, const StringRef lvlStr, MaybeWriter auto&&... msgs
  ) noexcept {
    logln(
        level,
        [&lvlStr](const StringRef func) noexcept -> String {
          return fmt::format(
              "{}Poac {} {}{} ", gray("["), lvlStr, func, gray("]")
          );
        },
        std::forward<decltype(msgs)>(msgs)...
    );
  }

  template <typename Fn>
    requires(std::is_nothrow_invocable_r_v<String, Fn, StringRef>)
  static void logln(
      Level level, Fn&& processHead, Writer auto&& writer, Display auto&& head,
      Display auto&&... msgs
  ) noexcept {
    loglnImpl(
        std::forward<decltype(writer)>(writer), level,
        std::forward<Fn>(processHead)(std::forward<decltype(head)>(head)),
        std::forward<decltype(msgs)>(msgs)...
    );
  }
  template <typename Fn>
    requires(std::is_nothrow_invocable_r_v<String, Fn, StringRef>)
  static void logln(
      Level level, Fn&& processHead, Display auto&& head, Display auto&&... msgs
  ) noexcept {
    loglnImpl(
        std::cerr, level,
        std::forward<Fn>(processHead)(std::forward<decltype(head)>(head)),
        std::forward<decltype(msgs)>(msgs)...
    );
  }

  static inline void
  loglnImpl(std::ostream& os, Level level, Display auto&&... msgs) noexcept {
    instance().log(os, level, std::forward<decltype(msgs)>(msgs)..., '\n');
  }

  inline void
  log(std::ostream& os, Level level, Display auto&&... msgs) noexcept {
    if (level <= this->level) {
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

template <typename... Ts>
  requires(MaybeWriter<Ts> && ...)
struct debug { // NOLINT(readability-identifier-naming)
  explicit debug(
      Ts&&... msgs, const source_location& loc = source_location::current()
  ) noexcept {
    Logger::debug(loc.function_name(), std::forward<Ts>(msgs)...);
  }
};
template <typename... Ts>
  requires(MaybeWriter<Ts> && ...)
debug(Ts&&...) -> debug<Ts...>;

template <typename... Ts>
  requires(MaybeWriter<Ts> && ...)
struct trace { // NOLINT(readability-identifier-naming)
  explicit trace(
      Ts&&... msgs, const source_location& loc = source_location::current()
  ) noexcept {
    Logger::debug(loc.function_name(), std::forward<Ts>(msgs)...);
  }
};
template <typename... Ts>
  requires(MaybeWriter<Ts> && ...)
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
