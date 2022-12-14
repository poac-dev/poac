#pragma once

// std
#include <memory>
#include <string>
#include <utility>

// external
#include <fmt/core.h> // NOLINT(build/include_order)
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/result/result.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "poac/util/rustify.hpp"
#include "poac/util/termcolor2/literals_extra.hpp"
#include "poac/util/termcolor2/termcolor2.hpp"

//
// Macros
//
// NOLINTNEXTLINE(readability-identifier-naming)
#define Try(...) MITAMA_TRY(__VA_ARGS__)

// NOLINTNEXTLINE(readability-identifier-naming)
#define alias_fn(lhs, rhs)                                                    \
  template <typename... Args>                                                 \
  inline Fn lhs(Args&&... args)->decltype(rhs(std::forward<Args>(args)...)) { \
    return rhs(std::forward<Args>(args)...);                                  \
  }

namespace poac {

//
// Namespaces
//
namespace anyhow = mitama::anyhow;
namespace thiserror = mitama::thiserror;

//
// Data types
//
template <typename T, typename E = void>
using Result = std::conditional_t<
    std::is_void_v<E>, anyhow::result<T>, mitama::result<T, E>>;

alias_fn(Ok, mitama::success); // NOLINT(readability-identifier-naming)

template <typename E = void, typename... Args>
inline Fn Err(Args&&... args) { // NOLINT(readability-identifier-naming)
  if constexpr (std::is_void_v<E>) {
    return mitama::failure(std::forward<Args>(args)...);
  } else {
    return anyhow::failure<E>(std::forward<Args>(args)...);
  }
}

//
// String literals
//
using namespace fmt::literals;
using namespace termcolor2::color_literals; // NOLINT(build/namespaces)

//
// Utilities
//
using fmt::format;
using fmt::print;

// NOLINTNEXTLINE(readability-identifier-naming)
inline constexpr auto to_anyhow = [](const String& e) {
  return anyhow::anyhow(e);
};

template <thiserror::fixed_string S, class... T>
using Error = thiserror::error<S, T...>;

//
// Errors
//
using SubprocessFailed = Error<"`{}` completed with exit code {}", String, i32>;

//
// Logs
//
namespace log {

  // Printed when `--verbose`
  template <typename T>
  inline void debug(T&& msg) {
    spdlog::debug("[poac] {}", std::forward<T>(msg));
  }
  template <typename... Args>
  inline void debug(fmt::format_string<Args...> fmt, Args&&... args) {
    debug(format(fmt, std::forward<Args>(args)...));
  }

  // Printed when `no option` & `--verbose`
  template <typename T>
  inline void status(StringRef header, T&& msg) {
    if (termcolor2::should_color()) {
      spdlog::info(
          "{:>27} {}", termcolor2::to_bold_green(header), std::forward<T>(msg)
      );
    } else {
      spdlog::info("{:>12} {}", header, std::forward<T>(msg));
    }
  }
  template <typename... Args>
  inline void
  status(StringRef header, fmt::format_string<Args...> fmt, Args&&... args) {
    status(header, format(fmt, std::forward<Args>(args)...));
  }

  template <typename T>
  inline void warn(T&& msg) {
    spdlog::warn("{} {}", "Warning:"_bold_yellow, std::forward<T>(msg));
  }
  template <typename... Args>
  inline void warn(fmt::format_string<Args...> fmt, Args&&... args) {
    warn(format(fmt, std::forward<Args>(args)...));
  }

  template <typename T>
  inline void error(const std::shared_ptr<spdlog::logger>& logger, T&& msg) {
    logger->error("{} {}", "Error:"_bold_red, std::forward<T>(msg));
  }
  template <typename... Args>
  inline void error(
      const std::shared_ptr<spdlog::logger>& logger,
      fmt::format_string<Args...> fmt, Args&&... args
  ) {
    error(logger, format(fmt, std::forward<Args>(args)...));
  }

} // namespace log

} // namespace poac

//
// Custom formatters
//
#if FMT_VERSION < 90000

namespace fmt {

template <>
struct formatter<std::string_view> {
  constexpr Fn parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline Fn format(std::string_view sv, FormatContext& ctx) {
    return format_to(ctx.out(), "{}", std::string(sv));
  }
};

template <>
struct formatter<std::filesystem::path> {
  constexpr Fn parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline Fn format(const std::filesystem::path& p, FormatContext& ctx) {
    return format_to(ctx.out(), "{}", p.string());
  }
};

template <typename T1, typename T2>
struct formatter<std::pair<T1, T2>> {
  constexpr Fn parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline Fn format(const std::pair<T1, T2>& p, FormatContext& ctx) {
    return format_to(ctx.out(), "({}, {})", p.first, p.second);
  }
};

} // namespace fmt

#else

#  include <fmt/std.h>

#endif
