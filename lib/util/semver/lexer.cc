// internal
#include "poac/util/semver/lexer.hpp"

namespace semver {

auto Lexer::next() -> Token {
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

void Lexer::step_n(const size_type& n) noexcept {
  for (size_type i = 0; i < n; ++i) {
    step();
  }
}

/// Consume a component.
///
/// A component can either be an alphanumeric or numeric.
/// Does not permit leading zeroes if numeric.
auto Lexer::component() -> Token {
  // e.g. abcde
  if (is_alphabet(this->one())) {
    const size_type start = this->c1_index;
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

  const size_type start = this->c1_index;
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
auto Lexer::whitespace() noexcept -> Token {
  const size_type start = this->c1_index;
  while (is_whitespace(this->one())) {
    this->step();
  }
  return Token{Token::Whitespace, start, this->c1_index};
}

} // end namespace semver
