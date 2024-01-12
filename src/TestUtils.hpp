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
std::ostream& operator<<(std::ostream& os, const std::optional<T>& opt) {
  if (opt.has_value()) {
    os << opt.value();
  } else {
    os << "None";
  }
  return os;
}

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

#define REGISTER_TEST(name)                                                   \
  name();                                                                     \
  std::cout << "test " << __FILE__ << "::" << #name << " ... " << green("ok") \
            << '\n'                                                           \
            << std::flush

template <typename... Ts>
  requires(Printable<Ts> && ...)
inline void reportError(
    const StringRef f,
    const int l,
    const StringRef fn,
    Ts&&... msgs
) noexcept {
  std::cerr << "test " << f << "::" << fn << " ... " << red("FAILED") << "\n\n"
            << '\'' << fn << "' failed at '" << std::boolalpha;
  (std::cerr << ... << std::forward<Ts>(msgs))
      << "', " << f << ':' << l << '\n';
}

inline void assertTrue(
    const bool cond,
    const StringRef msg = "",
    const StringRef f = __builtin_FILE(),
    const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (cond) {
    return; // OK
  }

  if (msg.empty()) {
    reportError(f, l, fn, "expected true: `", cond, '`');
  } else {
    reportError(f, l, fn, msg);
  }
  std::exit(EXIT_FAILURE);
}

inline void assertFalse(
    const bool cond,
    const StringRef msg = "",
    const StringRef f = __builtin_FILE(),
    const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (!cond) {
    return; // OK
  }

  if (msg.empty()) {
    reportError(f, l, fn, "expected false: `", cond, '`');
  } else {
    reportError(f, l, fn, msg);
  }
  std::exit(EXIT_FAILURE);
}

template <typename Lhs, typename Rhs>
  requires(Eq<Lhs, Rhs> && Printable<Lhs> && Printable<Rhs>)
inline void assertEq(
    Lhs&& lhs,
    Rhs&& rhs,
    const StringRef msg = "",
    const StringRef f = __builtin_FILE(),
    const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (lhs == rhs) {
    return; // OK
  }

  if (msg.empty()) {
    reportError(
        f,
        l,
        fn,
        "assertion failed: `(left == right)`\n",
        "  left: `",
        std::forward<Lhs>(lhs),
        "`\n",
        " right: `",
        std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    reportError(f, l, fn, msg);
  }
  std::exit(EXIT_FAILURE);
}

template <typename Lhs, typename Rhs>
  requires(Ne<Lhs, Rhs> && Printable<Lhs> && Printable<Rhs>)
inline void assertNe(
    Lhs&& lhs,
    Rhs&& rhs,
    const StringRef msg = "",
    const StringRef f = __builtin_FILE(),
    const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (lhs != rhs) {
    return; // OK
  }

  if (msg.empty()) {
    reportError(
        f,
        l,
        fn,
        "assertion failed: `(left != right)`\n",
        "  left: `",
        std::forward<Lhs>(lhs),
        "`\n",
        " right: `",
        std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    reportError(f, l, fn, msg);
  }
  std::exit(EXIT_FAILURE);
}

template <typename Lhs, typename Rhs>
  requires(Lt<Lhs, Rhs> && Printable<Lhs> && Printable<Rhs>)
inline void assertLt(
    Lhs&& lhs,
    Rhs&& rhs,
    const StringRef msg = "",
    const StringRef f = __builtin_FILE(),
    const int l = __builtin_LINE(),
    const StringRef fn = __builtin_FUNCTION()
) noexcept {
  if (lhs < rhs) {
    return; // OK
  }

  if (msg.empty()) {
    reportError(
        f,
        l,
        fn,
        "assertion failed: `(left < right)`\n",
        "  left: `",
        std::forward<Lhs>(lhs),
        "`\n",
        " right: `",
        std::forward<Rhs>(rhs),
        "`\n"
    );
  } else {
    reportError(f, l, fn, msg);
  }
  std::exit(EXIT_FAILURE);
}

template <typename E, typename Fn>
  requires(std::is_invocable_v<Fn>)
inline void assertException(
    Fn&& fn,
    const StringRef msg,
    const StringRef f = __builtin_FILE(),
    const int l = __builtin_LINE(),
    const StringRef fnName = __builtin_FUNCTION()
) noexcept {
  try {
    std::forward<Fn>(fn)();
    reportError(
        f, l, fnName, "expected exception `", typeid(E).name(), "` not thrown"
    );
    std::exit(EXIT_FAILURE);
  } catch (const E& e) {
    if (e.what() == String(msg)) {
      return; // OK
    }

    reportError(
        f,
        l,
        fnName,
        "expected exception message `",
        msg,
        "` but got `",
        e.what(),
        '`'
    );
    std::exit(EXIT_FAILURE);
  } catch (...) {
    reportError(
        f,
        l,
        fnName,
        "expected exception `",
        typeid(E).name(),
        "` but got `",
        typeid(std::current_exception()).name(),
        '`'
    );
    std::exit(EXIT_FAILURE);
  }
}

template <typename Fn>
  requires(std::is_invocable_v<Fn>)
inline void assertNoException(
    Fn&& fn,
    const StringRef f = __builtin_FILE(),
    const int l = __builtin_LINE(),
    const StringRef fnName = __builtin_FUNCTION()
) noexcept {
  try {
    std::forward<Fn>(fn)();
    // OK
  } catch (...) {
    reportError(
        f,
        l,
        fnName,
        "unexpected exception `",
        typeid(std::current_exception()).name(),
        '`'
    );
    std::exit(EXIT_FAILURE);
  }
}
