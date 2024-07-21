#pragma once

#include "Aliases.hpp"
#include "Traits.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace tests {

inline constinit const std::string_view GREEN = "\033[32m";
inline constinit const std::string_view RED = "\033[31m";
inline constinit const std::string_view RESET = "\033[0m";

inline constinit const usize SRC_REL_PATH_LEN = 6; // `../../`

// Returns the module name from a file path.  There are two cases:
//
// 1. src/Rustify/Tests.cc -> Rustify/Tests
//    (when we run `make test` from the root directory)
// 2. ../../src/Rustify/Tests.cc -> Rustify/Tests
//    (when we run `poac test`)
//
// We first should remove `../../` if it exists, then remove the first
// directory name since it can be either `src` or `tests`.  Finally, we remove
// the file extension, which is basically any of C++ source file extensions.
inline constexpr std::string_view
modName(std::string_view file) noexcept {
  if (file.empty()) {
    return file;
  }

  if (file.starts_with("..")) {
    file = file.substr(SRC_REL_PATH_LEN + 1);
  }

  usize start = file.find_first_of('/');
  if (start == std::string_view::npos) {
    return file;
  }
  ++start;

  const usize end = file.find_last_of('.');
  if (end == std::string_view::npos) {
    return file;
  }

  return file.substr(start, end - start);
}

inline void
pass(const source_location& loc = source_location::current()) noexcept {
  std::cout << "test " << modName(loc.file_name())
            << "::" << loc.function_name() << " ... " << GREEN << "ok" << RESET
            << '\n'
            << std::flush;
}

[[noreturn]] inline void
error(const source_location& loc, Display auto&&... msgs) noexcept {
  std::cerr << "test " << modName(loc.file_name())
            << "::" << loc.function_name() << " ... " << RED << "FAILED"
            << RESET << "\n\n"
            << '\'' << loc.function_name() << "' failed at '" << std::boolalpha;
  (std::cerr << ... << std::forward<decltype(msgs)>(msgs))
      << "', " << loc.file_name() << ':' << loc.line() << '\n';
  std::exit(EXIT_FAILURE);
}

inline void
assertTrue(
    const bool cond, const std::string_view msg = "",
    const source_location& loc = source_location::current()
) noexcept {
  if (cond) {
    return; // OK
  }

  if (msg.empty()) {
    error(loc, "expected `true` but got `false`");
  } else {
    error(loc, msg);
  }
}

inline void
assertFalse(
    const bool cond, const std::string_view msg = "",
    const source_location& loc = source_location::current()
) noexcept {
  if (!cond) {
    return; // OK
  }

  if (msg.empty()) {
    error(loc, "expected `false` but got `true`");
  } else {
    error(loc, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Eq<Lhs, Rhs> && Display<Lhs> && Display<Rhs>)
inline void
assertEq(
    Lhs&& lhs, Rhs&& rhs, const std::string_view msg = "",
    const source_location& loc = source_location::current()
) noexcept {
  if (lhs == rhs) {
    return; // OK
  }

  if (msg.empty()) {
    error(
        loc, "assertion failed: `(left == right)`\n", "  left: `",
        std::forward<Lhs>(lhs), "`\n", " right: `", std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    error(loc, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Ne<Lhs, Rhs> && Display<Lhs> && Display<Rhs>)
inline void
assertNe(
    Lhs&& lhs, Rhs&& rhs, const std::string_view msg = "",
    const source_location& loc = source_location::current()
) noexcept {
  if (lhs != rhs) {
    return; // OK
  }

  if (msg.empty()) {
    error(
        loc, "assertion failed: `(left != right)`\n", "  left: `",
        std::forward<Lhs>(lhs), "`\n", " right: `", std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    error(loc, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Lt<Lhs, Rhs> && Display<Lhs> && Display<Rhs>)
inline void
assertLt(
    Lhs&& lhs, Rhs&& rhs, const std::string_view msg = "",
    const source_location& loc = source_location::current()
) noexcept {
  if (lhs < rhs) {
    return; // OK
  }

  if (msg.empty()) {
    error(
        loc, "assertion failed: `(left < right)`\n", "  left: `",
        std::forward<Lhs>(lhs), "`\n", " right: `", std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    error(loc, msg);
  }
}

template <typename E, typename Fn>
  requires(std::is_invocable_v<Fn>)
inline void
assertException(
    Fn&& func, const std::string_view msg,
    const source_location& loc = source_location::current()
) noexcept {
  try {
    std::forward<Fn>(func)();
    error(loc, "expected exception `", typeid(E).name(), "` not thrown");
  } catch (const E& e) {
    if (e.what() == std::string(msg)) {
      return; // OK
    }

    error(
        loc, "expected exception message `", msg, "` but got `", e.what(), '`'
    );
  } catch (...) {
    error(
        loc, "expected exception `", typeid(E).name(), "` but got `",
        typeid(std::current_exception()).name(), '`'
    );
  }
}

template <typename Fn>
  requires(std::is_invocable_v<Fn>)
inline void
assertNoException(
    Fn&& func, const source_location& loc = source_location::current()
) noexcept {
  try {
    std::forward<Fn>(func)();
    // OK
  } catch (...) {
    error(
        loc, "unexpected exception `", typeid(std::current_exception()).name(),
        '`'
    );
  }
}

} // namespace tests
