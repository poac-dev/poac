#pragma once

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <ostream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace fs = std::filesystem;

// NOLINTBEGIN(readability-identifier-naming)
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using isize = std::ptrdiff_t;
using usize = std::size_t;

using f32 = float;
using f64 = double;
// NOLINTEND(readability-identifier-naming)

template <typename T>
using Vec = std::vector<T>;

template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template <typename K>
using HashSet = std::unordered_set<K>;

template <typename T>
using Option = std::optional<T>;

struct NoneT : protected std::monostate {
  constexpr bool operator==(const usize rhs) const {
    return std::string_view::npos == rhs;
  }

  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr operator std::nullopt_t() const {
    return std::nullopt;
  }

  template <typename T>
  constexpr operator Option<T>() const { // NOLINT(google-explicit-constructor)
    return std::nullopt;
  }
};
inline constinit const NoneT None; // NOLINT(readability-identifier-naming)

inline std::ostream&
operator<<(std::ostream& os, const NoneT& /*unused*/) {
  return os << "None";
}

// NOLINTBEGIN(google-global-names-in-headers)
using std::literals::string_literals::operator""s;
using std::literals::string_view_literals::operator""sv;
// NOLINTEND(google-global-names-in-headers)

inline fs::path
operator""_path(const char* str, usize /*unused*/) {
  return str;
}

// NOLINTBEGIN(readability-identifier-naming)
struct source_location {
  constexpr source_location() noexcept = delete;
  constexpr ~source_location() noexcept = default;
  constexpr source_location(const source_location&) noexcept = default;
  constexpr source_location(source_location&&) noexcept = default;
  constexpr source_location&
  operator=(const source_location&) noexcept = default;
  constexpr source_location& operator=(source_location&&) noexcept = default;

  constexpr source_location(
      const char* file, int line, const char* func
  ) noexcept
      : file_(file), line_(line), func_(func) {}

  static constexpr source_location current(
      const char* file = __builtin_FILE(), const int line = __builtin_LINE(),
      const char* func = __builtin_FUNCTION()
  ) noexcept {
    return { file, line, func };
  }
  constexpr std::string_view file_name() const noexcept {
    return file_;
  }
  constexpr int line() const noexcept {
    return line_;
  }
  constexpr std::string_view function_name() const noexcept {
    return func_;
  }

private:
  const char* file_;
  int line_{};
  const char* func_;
};
// NOLINTEND(readability-identifier-naming)

[[noreturn]] inline void
panic(
    const std::string_view msg,
    const source_location& loc = source_location::current()
) noexcept {
  std::cerr << "panicked at '" << msg << "', " << loc.file_name() << ':'
            << loc.line() << '\n';
  std::exit(EXIT_FAILURE);

  // TODO: throw an exception instead?
}

[[noreturn]] inline void
unreachable(
    [[maybe_unused]] const source_location& loc = source_location::current()
) noexcept {
#ifdef NDEBUG
  __builtin_unreachable();
#else
  panic("unreachable", loc);
#endif
}
