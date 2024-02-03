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

namespace detail {

  class Logger {
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
      requires((Writer<Ts> || Display<Ts>) && ...)
    static void error(Ts&&... msgs) noexcept {
      logln(
          Level::Error,
          [](const StringRef head) noexcept { return String(head); },
          bold(red("Error: ")), std::forward<Ts>(msgs)...
      );
    }

    template <typename... Ts>
      requires((Writer<Ts> || Display<Ts>) && ...)
    static void warn(Ts&&... msgs) noexcept {
      logln(
          Level::Warn,
          [](const StringRef head) noexcept { return String(head); },
          bold(yellow("Warning: ")), std::forward<Ts>(msgs)...
      );
    }

    template <typename... Ts>
      requires((Writer<Ts> || Display<Ts>) && ...)
    static void info(Ts&&... msgs) noexcept {
      constexpr auto processHead = [](const StringRef head) noexcept -> String {
        const StringRef fmtStr = shouldColor() ? "{:>21} " : "{:>12} ";
        return fmt::format(fmt::runtime(fmtStr), bold(green(head)));
      };
      logln(Level::Info, processHead, std::forward<Ts>(msgs)...);
    }

    template <typename... Ts>
      requires((Writer<Ts> || Display<Ts>) && ...)
    static void debug(Ts&&... msgs) noexcept {
      debuglike(Level::Debug, blue("DEBUG"), std::forward<Ts>(msgs)...);
    }

    template <typename... Ts>
      requires((Writer<Ts> || Display<Ts>) && ...)
    static void trace(Ts&&... msgs) noexcept {
      debuglike(Level::Trace, cyan("TRACE"), std::forward<Ts>(msgs)...);
    }

  private:
    template <typename... Ts>
    static void
    debuglike(Level level, const StringRef lvlStr, Ts&&... msgs) noexcept {
      const auto processHead = //
          [&lvlStr](const StringRef func) noexcept -> String {
        return fmt::format(
            "{}Poac {} {}{} ", gray("["), lvlStr, func, gray("]")
        );
      };
      logln(level, processHead, std::forward<Ts>(msgs)...);
    }

    template <typename Fn, typename T, typename U, typename... Ts>
      requires(
          std::is_nothrow_invocable_r_v<String, Fn, StringRef>
          && (Writer<T> || Display<T>) && Display<U> && (Display<Ts> && ...)
      )
    static void logln(
        Level level, Fn&& processHead, T&& writerOrHead, U&& headOrMsg,
        Ts&&... msgs
    ) noexcept {
      if constexpr (Writer<T>) {
        loglnImpl(
            std::forward<T>(writerOrHead), level,
            std::forward<Fn>(processHead)(std::forward<U>(headOrMsg)),
            std::forward<Ts>(msgs)...
        );
      } else {
        loglnImpl(
            std::cerr, level,
            std::forward<Fn>(processHead)(std::forward<T>(writerOrHead)),
            std::forward<U>(headOrMsg), std::forward<Ts>(msgs)...
        );
      }
    }

    template <typename... Ts>
      requires(Display<Ts> && ...)
    static void
    loglnImpl(std::ostream& os, Level level, Ts&&... msgs) noexcept {
      instance().log(os, level, std::forward<Ts>(msgs)..., '\n');
    }

    template <typename... Ts>
      requires(Display<Ts> && ...)
    void log(std::ostream& os, Level level, Ts&&... msgs) noexcept {
      if (level <= this->level) {
        (os << ... << std::forward<Ts>(msgs)) << std::flush;
      }
    }
  };

} // namespace detail

template <typename... Ts>
  requires((Writer<Ts> || Display<Ts>) && ...)
void
error(Ts&&... msgs) noexcept {
  detail::Logger::error(std::forward<Ts>(msgs)...);
}

template <typename... Ts>
  requires((Writer<Ts> || Display<Ts>) && ...)
void
warn(Ts&&... msgs) noexcept {
  detail::Logger::warn(std::forward<Ts>(msgs)...);
}

template <typename... Ts>
  requires((Writer<Ts> || Display<Ts>) && ...)
void
info(Ts&&... msgs) noexcept {
  detail::Logger::info(std::forward<Ts>(msgs)...);
}

template <typename... Ts>
  requires((Writer<Ts> || Display<Ts>) && ...)
struct debug { // NOLINT(readability-identifier-naming)
  explicit debug(
      Ts&&... msgs, const source_location& loc = source_location::current()
  ) noexcept {
    detail::Logger::debug(loc.function_name(), std::forward<Ts>(msgs)...);
  }
};
template <typename... Ts>
  requires((Writer<Ts> || Display<Ts>) && ...)
debug(Ts&&...) -> debug<Ts...>;

template <typename... Ts>
  requires((Writer<Ts> || Display<Ts>) && ...)
struct trace { // NOLINT(readability-identifier-naming)
  explicit trace(
      Ts&&... msgs, const source_location& loc = source_location::current()
  ) noexcept {
    detail::Logger::debug(loc.function_name(), std::forward<Ts>(msgs)...);
  }
};
template <typename... Ts>
  requires((Writer<Ts> || Display<Ts>) && ...)
trace(Ts&&...) -> trace<Ts...>;

void setLevel(Level level) noexcept;
Level getLevel() noexcept;

} // namespace logger

bool isVerbose() noexcept;
bool isQuiet() noexcept;
