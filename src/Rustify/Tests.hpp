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
modName(StringRef f) noexcept {
  if (f.empty()) {
    return f;
  }

  if (f.starts_with("..")) {
    f = f.substr(7);
  }

  usize start = f.find_first_of('/');
  if (start == StringRef::npos) {
    return f;
  }
  ++start;

  const usize end = f.find_last_of('.');
  if (end == StringRef::npos) {
    return f;
  }

  return f.substr(start, end - start);
}

inline void
pass(
    const StringRef f = __builtin_FILE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  std::cout << "test " << modName(f) << "::" << fn << " ... " << GREEN << "ok"
            << RESET << '\n'
            << std::flush;
}

template <typename... Ts>
  requires(Display<Ts> && ...)
[[noreturn]] inline void
error(
    const StringRef f, const int l, const StringRef fn, Ts&&... msgs
) noexcept {
  std::cerr << "test " << modName(f) << "::" << fn << " ... " << RED << "FAILED"
            << RESET << "\n\n"
            << '\'' << fn << "' failed at '" << std::boolalpha;
  (std::cerr << ... << std::forward<Ts>(msgs))
      << "', " << f << ':' << l << '\n';
  std::exit(EXIT_FAILURE);
}

inline void
assertTrue(
    const bool cond, const StringRef msg = "",
    const StringRef f = __builtin_FILE(), const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (cond) {
    return; // OK
  }

  if (msg.empty()) {
    error(f, l, fn, "expected `true` but got `false`");
  } else {
    error(f, l, fn, msg);
  }
}

inline void
assertFalse(
    const bool cond, const StringRef msg = "",
    const StringRef f = __builtin_FILE(), const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (!cond) {
    return; // OK
  }

  if (msg.empty()) {
    error(f, l, fn, "expected `false` but got `true`");
  } else {
    error(f, l, fn, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Eq<Lhs, Rhs> && Display<Lhs> && Display<Rhs>)
inline void
assertEq(
    Lhs&& lhs, Rhs&& rhs, const StringRef msg = "",
    const StringRef f = __builtin_FILE(), const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (lhs == rhs) {
    return; // OK
  }

  if (msg.empty()) {
    error(
        f, l, fn, "assertion failed: `(left == right)`\n", "  left: `",
        std::forward<Lhs>(lhs), "`\n", " right: `", std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    error(f, l, fn, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Ne<Lhs, Rhs> && Display<Lhs> && Display<Rhs>)
inline void
assertNe(
    Lhs&& lhs, Rhs&& rhs, const StringRef msg = "",
    const StringRef f = __builtin_FILE(), const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (lhs != rhs) {
    return; // OK
  }

  if (msg.empty()) {
    error(
        f, l, fn, "assertion failed: `(left != right)`\n", "  left: `",
        std::forward<Lhs>(lhs), "`\n", " right: `", std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    error(f, l, fn, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Lt<Lhs, Rhs> && Display<Lhs> && Display<Rhs>)
inline void
assertLt(
    Lhs&& lhs, Rhs&& rhs, const StringRef msg = "",
    const StringRef f = __builtin_FILE(), const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (lhs < rhs) {
    return; // OK
  }

  if (msg.empty()) {
    error(
        f, l, fn, "assertion failed: `(left < right)`\n", "  left: `",
        std::forward<Lhs>(lhs), "`\n", " right: `", std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    error(f, l, fn, msg);
  }
}

template <typename E, typename Fn>
  requires(std::is_invocable_v<Fn>)
inline void
assertException(
    Fn&& fn, const StringRef msg, const StringRef f = __builtin_FILE(),
    const int l = __builtin_LINE(),
    const StringRef fnName = __builtin_FUNCTION()
) noexcept {
  try {
    std::forward<Fn>(fn)();
    error(
        f, l, fnName, "expected exception `", typeid(E).name(), "` not thrown"
    );
  } catch (const E& e) {
    if (e.what() == String(msg)) {
      return; // OK
    }

    error(
        f, l, fnName, "expected exception message `", msg, "` but got `",
        e.what(), '`'
    );
  } catch (...) {
    error(
        f, l, fnName, "expected exception `", typeid(E).name(), "` but got `",
        typeid(std::current_exception()).name(), '`'
    );
  }
}

template <typename Fn>
  requires(std::is_invocable_v<Fn>)
inline void
assertNoException(
    Fn&& fn, const StringRef f = __builtin_FILE(),
    const int l = __builtin_LINE(),
    const StringRef fnName = __builtin_FUNCTION()
) noexcept {
  try {
    std::forward<Fn>(fn)();
    // OK
  } catch (...) {
    error(
        f, l, fnName, "unexpected exception `",
        typeid(std::current_exception()).name(), '`'
    );
  }
}

} // namespace tests
