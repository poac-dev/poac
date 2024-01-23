#pragma once

#include "Aliases.hpp"
#include "Traits.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace tests {

inline constinit const StringRef GREEN = "\033[32m";
inline constinit const StringRef RED = "\033[31m";
inline constinit const StringRef RESET = "\033[0m";

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
inline StringRef
modName(StringRef file) noexcept {
  if (file.empty()) {
    return file;
  }

  if (file.starts_with("..")) {
    file = file.substr(7);
  }

  usize start = file.find_first_of('/');
  if (start == StringRef::npos) {
    return file;
  }
  ++start;

  const usize end = file.find_last_of('.');
  if (end == StringRef::npos) {
    return file;
  }

  return file.substr(start, end - start);
}

inline void
pass(
    const StringRef file = __builtin_FILE(),
    const StringRef func = __builtin_FUNCTION()
) noexcept {
  std::cout << "test " << modName(file) << "::" << func << " ... " << GREEN
            << "ok" << RESET << '\n'
            << std::flush;
}

template <typename... Ts>
  requires(Display<Ts> && ...)
[[noreturn]] inline void
error(
    const StringRef file, const int line, const StringRef func, Ts&&... msgs
) noexcept {
  std::cerr << "test " << modName(file) << "::" << func << " ... " << RED
            << "FAILED" << RESET << "\n\n"
            << '\'' << func << "' failed at '" << std::boolalpha;
  (std::cerr << ... << std::forward<Ts>(msgs))
      << "', " << file << ':' << line << '\n';
  std::exit(EXIT_FAILURE);
}

inline void
assertTrue(
    const bool cond, const StringRef msg = "",
    const StringRef file = __builtin_FILE(), const int line = __builtin_LINE(),
    const StringRef func = __builtin_FUNCTION()
) noexcept {
  if (cond) {
    return; // OK
  }

  if (msg.empty()) {
    error(file, line, func, "expected `true` but got `false`");
  } else {
    error(file, line, func, msg);
  }
}

inline void
assertFalse(
    const bool cond, const StringRef msg = "",
    const StringRef file = __builtin_FILE(), const int line = __builtin_LINE(),
    const StringRef func = __builtin_FUNCTION()
) noexcept {
  if (!cond) {
    return; // OK
  }

  if (msg.empty()) {
    error(file, line, func, "expected `false` but got `true`");
  } else {
    error(file, line, func, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Eq<Lhs, Rhs> && Display<Lhs> && Display<Rhs>)
inline void
assertEq(
    Lhs&& lhs, Rhs&& rhs, const StringRef msg = "",
    const StringRef file = __builtin_FILE(), const int line = __builtin_LINE(),
    const StringRef func = __builtin_FUNCTION()
) noexcept {
  if (lhs == rhs) {
    return; // OK
  }

  if (msg.empty()) {
    error(
        file, line, func, "assertion failed: `(left == right)`\n", "  left: `",
        std::forward<Lhs>(lhs), "`\n", " right: `", std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    error(file, line, func, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Ne<Lhs, Rhs> && Display<Lhs> && Display<Rhs>)
inline void
assertNe(
    Lhs&& lhs, Rhs&& rhs, const StringRef msg = "",
    const StringRef file = __builtin_FILE(), const int line = __builtin_LINE(),
    const StringRef func = __builtin_FUNCTION()
) noexcept {
  if (lhs != rhs) {
    return; // OK
  }

  if (msg.empty()) {
    error(
        file, line, func, "assertion failed: `(left != right)`\n", "  left: `",
        std::forward<Lhs>(lhs), "`\n", " right: `", std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    error(file, line, func, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Lt<Lhs, Rhs> && Display<Lhs> && Display<Rhs>)
inline void
assertLt(
    Lhs&& lhs, Rhs&& rhs, const StringRef msg = "",
    const StringRef file = __builtin_FILE(), const int line = __builtin_LINE(),
    const StringRef func = __builtin_FUNCTION()
) noexcept {
  if (lhs < rhs) {
    return; // OK
  }

  if (msg.empty()) {
    error(
        file, line, func, "assertion failed: `(left < right)`\n", "  left: `",
        std::forward<Lhs>(lhs), "`\n", " right: `", std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    error(file, line, func, msg);
  }
}

template <typename E, typename Fn>
  requires(std::is_invocable_v<Fn>)
inline void
assertException(
    Fn&& func, const StringRef msg, const StringRef file = __builtin_FILE(),
    const int line = __builtin_LINE(),
    const StringRef funcName = __builtin_FUNCTION()
) noexcept {
  try {
    std::forward<Fn>(func)();
    error(
        file, line, funcName, "expected exception `", typeid(E).name(),
        "` not thrown"
    );
  } catch (const E& e) {
    if (e.what() == String(msg)) {
      return; // OK
    }

    error(
        file, line, funcName, "expected exception message `", msg,
        "` but got `", e.what(), '`'
    );
  } catch (...) {
    error(
        file, line, funcName, "expected exception `", typeid(E).name(),
        "` but got `", typeid(std::current_exception()).name(), '`'
    );
  }
}

template <typename Fn>
  requires(std::is_invocable_v<Fn>)
inline void
assertNoException(
    Fn&& func, const StringRef file = __builtin_FILE(),
    const int line = __builtin_LINE(),
    const StringRef funcName = __builtin_FUNCTION()
) noexcept {
  try {
    std::forward<Fn>(func)();
    // OK
  } catch (...) {
    error(
        file, line, funcName, "unexpected exception `",
        typeid(std::current_exception()).name(), '`'
    );
  }
}

} // namespace tests
