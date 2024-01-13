#pragma once

#include "Rustify.hpp"
#include "TermColor.hpp"

#include <concepts>
#include <cstdlib>
#include <exception>
#include <ios>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

template <typename T>
std::ostream&
operator<<(std::ostream& os, const std::optional<T>& opt) {
  if (opt.has_value()) {
    os << opt.value();
  } else {
    os << "None";
  }
  return os;
}

namespace tests {

template <typename T>
concept Printable = requires(T a, std::ostream& os) {
  { os << a } -> std::convertible_to<std::ostream&>;
};

template <typename T, typename U>
concept Eq = requires(T a, U b) {
  { a == b } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept Ne = requires(T a, U b) {
  { a != b } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept Lt = requires(T a, U b) {
  { a < b } -> std::convertible_to<bool>;
};

inline void
pass(
    const StringRef f = __builtin_FILE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  std::cout << "test " << f << "::" << fn << " ... " << green("ok") << '\n'
            << std::flush;
}

template <typename... Ts>
  requires(Printable<Ts> && ...)
[[noreturn]] inline void
error(
    const StringRef f, const int l, const StringRef fn, Ts&&... msgs
) noexcept {
  std::cerr << "test " << f << "::" << fn << " ... " << red("FAILED") << "\n\n"
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
    error(f, l, fn, "expected true: `", cond, '`');
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
    error(f, l, fn, "expected false: `", cond, '`');
  } else {
    error(f, l, fn, msg);
  }
}

template <typename Lhs, typename Rhs>
  requires(Eq<Lhs, Rhs> && Printable<Lhs> && Printable<Rhs>)
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
  requires(Ne<Lhs, Rhs> && Printable<Lhs> && Printable<Rhs>)
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
  requires(Lt<Lhs, Rhs> && Printable<Lhs> && Printable<Rhs>)
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
