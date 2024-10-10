#pragma once

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace fs = std::filesystem;

// NOLINTBEGIN(readability-identifier-naming)
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i32 = std::int32_t;
using i64 = std::int64_t;

using usize = std::size_t;
// NOLINTEND(readability-identifier-naming)

// NOLINTBEGIN(google-global-names-in-headers)
using std::literals::string_literals::operator""s;
using std::literals::string_view_literals::operator""sv;
// NOLINTEND(google-global-names-in-headers)

inline fs::path
operator""_path(const char* str, usize /*unused*/) {
  return str;
}

[[noreturn]] inline void
panic(
    const std::string_view msg,
    const std::source_location& loc = std::source_location::current()
) {
  std::ostringstream oss;
  oss << "panicked at '" << msg << "', " << loc.file_name() << ':' << loc.line()
      << '\n';
  throw std::logic_error(oss.str());
}

[[noreturn]] inline void
unreachable(
    [[maybe_unused]] const std::source_location& loc =
        std::source_location::current()
) noexcept {
#ifdef NDEBUG
  __builtin_unreachable();
#else
  panic("unreachable", loc);
#endif
}
