module;

// std
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

export module semver.lexer;

import semver.token;

namespace semver {

constexpr auto is_whitespace(const char c) noexcept -> bool {
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

constexpr auto is_digit(const char c) noexcept -> bool {
  return '0' <= c && c <= '9';
}

constexpr auto is_alphabet(const char c) noexcept -> bool {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

constexpr auto is_alpha_numeric(const char c) noexcept -> bool {
  return is_digit(c) || is_alphabet(c);
}

constexpr auto str_to_uint(std::string_view s) noexcept
    -> std::optional<std::uint_fast64_t> {
  std::uint_fast64_t i = 0;
  std::uint_fast64_t digit = 1;
  for (int size = s.size() - 1; size >= 0; --size) {
    const char c = s[size];
    if (is_digit(c)) {
      i += (c - '0') * digit;
    } else {
      return std::nullopt;
    }
    digit *= 10;
  }
  return i;
}

export class Lexer {
public:
  using SizeType = std::size_t;
  using StringType = std::string_view;
  using ValueType = StringType::value_type;
  using TraitsType = StringType::traits_type;
  using ConstIterator = StringType::const_iterator;
  using ConstReverseIterator = StringType::const_reverse_iterator;

  StringType str;
  SizeType c1_index{0};

  explicit Lexer(StringType s) : str(s) {}

  auto next() -> Token {
    // Check out of range
    if (c1_index
        >= this->size()) { // should be `>=`, not `>` because of this->two()
      return Token{Token::Unexpected};
    }

    // two subsequent char tokens.
    const auto [c1, c2] = this->two();
    if (c1 == '<' && c2 == '=') {
      this->step_n(2);
      return Token{Token::LtEq};
    } else if (c1 == '>' && c2 == '=') {
      this->step_n(2);
      return Token{Token::GtEq};
    } else if (c1 == '|' && c2 == '|') {
      this->step_n(2);
      return Token{Token::Or};
    }

    // single char and start of numeric tokens.
    if (is_whitespace(c1)) {
      return whitespace();
    } else if (c1 == '=') {
      this->step();
      return Token{Token::Eq};
    } else if (c1 == '>') {
      this->step();
      return Token{Token::Gt};
    } else if (c1 == '<') {
      this->step();
      return Token{Token::Lt};
    } else if (c1 == '^') {
      this->step();
      return Token{Token::Caret};
    } else if (c1 == '~') {
      this->step();
      return Token{Token::Tilde};
    } else if (c1 == '*') {
      this->step();
      return Token{Token::Star};
    } else if (c1 == '.') {
      this->step();
      return Token{Token::Dot};
    } else if (c1 == ',') {
      this->step();
      return Token{Token::Comma};
    } else if (c1 == '-') {
      this->step();
      return Token{Token::Hyphen};
    } else if (c1 == '+') {
      this->step();
      return Token{Token::Plus};
    } else if (is_alpha_numeric(c1)) {
      return component();
    } else {
      this->step();
      return Token{Token::Unexpected};
    }
  }

  [[nodiscard]] constexpr auto size() const noexcept -> SizeType {
    return str.size();
  }
  [[nodiscard]] constexpr auto max_size() const noexcept -> SizeType {
    return str.max_size();
  }
  [[nodiscard]] constexpr auto empty() const noexcept -> bool {
    return str.empty();
  }

private:
  inline void step() noexcept {
    ++c1_index;
  }

  void step_n(const SizeType& n) noexcept {
    for (SizeType i = 0; i < n; ++i) {
      step();
    }
  }

  /// Access the one character, or set it if it is not set.
  [[nodiscard]] inline auto one() const noexcept -> ValueType {
    return str[c1_index];
  }

  /// Access two characters.
  [[nodiscard]] inline auto two() const noexcept
      -> std::pair<ValueType, ValueType> {
    return {str[c1_index], str[c1_index + 1]};
  }

  /// Consume a component.
  ///
  /// A component can either be an alphanumeric or numeric.
  /// Does not permit leading zeroes if numeric.
  auto component() -> Token {
    // e.g. abcde
    if (is_alphabet(this->one())) {
      const SizeType start = this->c1_index;
      while (is_alpha_numeric(this->one())) {
        this->step();
      }
      const std::string_view sub = str.substr(start, this->c1_index - start);
      return Token{Token::AlphaNumeric, sub};
    }

    // exactly zero
    if (const auto [c1, c2] = this->two(); c1 == '0' && !is_digit(c2)) {
      this->step();
      return Token{Token::Numeric, 0};
    }

    const SizeType start = this->c1_index;
    while (is_digit(this->one())) {
      this->step();
    }
    if (str[start] != '0' && !is_alphabet(this->one())) {
      // e.g. 3425
      const std::string_view sub = str.substr(start, this->c1_index - start);
      const std::uint_fast64_t value = str_to_uint(sub).value();
      return Token{Token::Numeric, value};
    }

    // e.g. 3425dec85
    while (is_alpha_numeric(this->one())) {
      this->step();
    }
    const std::string_view sub = str.substr(start, this->c1_index - start);
    return Token{Token::AlphaNumeric, sub};
  }

  /// Consume whitespace.
  auto whitespace() -> Token {
    const SizeType start = this->c1_index;
    while (is_whitespace(this->one())) {
      this->step();
    }
    return Token{Token::Whitespace, start, this->c1_index};
  }
};

} // end namespace semver
