#pragma once

// std
#include <array>
#include <cstddef> // std::size_t
#include <cstdint>
#include <filesystem>
#include <functional> // std::equal_to
#include <iterator> // std::begin, std::end
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant> // std::monostate
#include <vector>

// external
#include <boost/container_hash/hash.hpp>
#include <fmt/core.h> // NOLINT(build/include_order)
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/result/result.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "poac/util/termcolor2/literals_extra.hpp"
#include "poac/util/termcolor2/termcolor2.hpp"

//
// Macros
//
// NOLINTNEXTLINE(readability-identifier-naming)
#define Fn auto
// NOLINTNEXTLINE(readability-identifier-naming)
#define Try(...) MITAMA_TRY(__VA_ARGS__)
// NOLINTNEXTLINE(readability-identifier-naming)
#define unreachable() __builtin_unreachable()

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
namespace fs = std::filesystem;
namespace anyhow = mitama::anyhow;
namespace thiserror = mitama::thiserror;

//
// Data types
//
using u8 = std::uint8_t; // NOLINT(readability-identifier-naming)
using u16 = std::uint16_t; // NOLINT(readability-identifier-naming)
using u32 = std::uint32_t; // NOLINT(readability-identifier-naming)
using u64 = std::uint64_t; // NOLINT(readability-identifier-naming)

using i8 = std::int8_t; // NOLINT(readability-identifier-naming)
using i16 = std::int16_t; // NOLINT(readability-identifier-naming)
using i32 = std::int32_t; // NOLINT(readability-identifier-naming)
using i64 = std::int64_t; // NOLINT(readability-identifier-naming)

using usize = std::size_t; // NOLINT(readability-identifier-naming)

using f32 = float; // NOLINT(readability-identifier-naming)
using f64 = double; // NOLINT(readability-identifier-naming)

using String = std::string;
using StringRef = std::string_view;
static_assert(String::npos == StringRef::npos, "npos should be the same");

using Path = fs::path;

template <typename T, usize N>
using Arr = std::array<T, N>;
template <typename T>
using Vec = std::vector<T>;

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

template <typename T>
using Option = std::optional<T>;

struct NoneT : protected std::monostate {
  constexpr Fn operator==(const usize rhs) const->bool {
    return String::npos == rhs;
  }

  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr operator std::nullopt_t() const { return std::nullopt; }

  template <typename T>
  constexpr operator Option<T>() const { // NOLINT(google-explicit-constructor)
    return std::nullopt;
  }
};
inline constexpr NoneT None; // NOLINT(readability-identifier-naming)

template <typename K, typename V>
using Map = std::map<K, V>;
template <typename K, typename V, typename H = boost::hash<K>>
using HashMap = std::unordered_map<K, V, H>;

template <typename K>
using HashSet = std::unordered_set<K, boost::hash<K>>;

//
// String literals
//
using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;
using namespace fmt::literals;
using namespace termcolor2::color_literals; // NOLINT(build/namespaces)

inline Fn operator""_path(const char* str, usize /*unused*/)->Path {
  return str;
}

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

template <typename T, typename U>
inline void append(Vec<T>& a, const Vec<U>& b) {
  a.insert(a.end(), b.cbegin(), b.cend());
}

template <typename K, typename V, typename H>
inline void append(HashMap<K, V, H>& a, const HashMap<K, V, H>& b) {
  a.insert(b.cbegin(), b.cend());
}

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
