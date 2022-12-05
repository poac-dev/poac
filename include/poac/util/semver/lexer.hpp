#ifndef POAC_UTIL_SEMVER_LEXER_HPP_
#define POAC_UTIL_SEMVER_LEXER_HPP_

// std
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

// internal
#include "poac/util/semver/token.hpp"

namespace semver {

constexpr bool is_whitespace(const char c) noexcept {
  switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      return true;
    default:
      return false;
  }
}

constexpr bool is_digit(const char c) noexcept { return '0' <= c && c <= '9'; }

constexpr bool is_alphabet(const char c) noexcept {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

constexpr bool is_alpha_numeric(const char c) noexcept {
  return is_digit(c) || is_alphabet(c);
}

constexpr std::optional<std::uint_fast64_t> str_to_uint(std::string_view s
) noexcept {
  std::uint_fast64_t i = 0;
  std::uint_fast64_t digit = 1;
  for (int size = s.size() - 1; size >= 0; --size) {
    char c = s[size];
    if (is_digit(c)) {
      i += (c - '0') * digit;
    } else {
      return std::nullopt;
    }
    digit *= 10;
  }
  return i;
}

class Lexer {
public:
  using size_type = std::size_t;
  using string_type = std::string_view;
  using value_type = string_type::value_type;
  using traits_type = string_type::traits_type;
  using const_iterator = string_type::const_iterator;
  using const_reverse_iterator = string_type::const_reverse_iterator;

  string_type str;
  size_type c1_index;

  explicit Lexer(string_type s) : str(s), c1_index(0) {}

  Token next();

  constexpr size_type size() const noexcept { return str.size(); }
  constexpr size_type max_size() const noexcept { return str.max_size(); }
  constexpr bool empty() const noexcept { return str.empty(); }

private:
  inline void step() noexcept { ++c1_index; }

  void step_n(const size_type& n) noexcept;

  /// Access the one character, or set it if it is not set.
  inline value_type one() const noexcept { return str[c1_index]; }

  /// Access two characters.
  inline std::pair<value_type, value_type> two() const noexcept {
    return {str[c1_index], str[c1_index + 1]};
  }

  /// Consume a component.
  ///
  /// A component can either be an alphanumeric or numeric.
  /// Does not permit leading zeroes if numeric.
  Token component();

  /// Consume whitespace.
  Token whitespace();
};

} // end namespace semver

#endif // POAC_UTIL_SEMVER_LEXER_HPP_
