#pragma once

// std
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

// internal
#include "poac/util/semver/exception.hpp"
#include "poac/util/semver/lexer.hpp"
#include "poac/util/semver/range.hpp"
#include "poac/util/semver/token.hpp"

namespace semver {

struct Parser {
  using StringType = std::string_view;
  using ValueType = StringType::value_type;

  Lexer lexer;
  Token c1;

  /// Construct a new parser for the given input.
  explicit Parser(StringType str) : lexer(str), c1(lexer.next()) {}

  /// Pop one token.
  inline auto pop() -> Token {
    Token c1_ = this->c1; // NOLINT(readability-identifier-naming)
    this->c1 = lexer.next();
    return c1_;
  }

  /// Peek one token.
  [[nodiscard]] inline auto peek() const noexcept -> Token { return this->c1; }

  /// Skip whitespace if present.
  void skip_whitespace();

  /// Parse an optional comma separator, then if that is present a predicate.
  auto comma_predicate() -> std::optional<Predicate>;

  /// Parse an optional or separator `||`, then if that is present a range.
  auto or_range() -> std::optional<VersionReq>;

  /// Parse a single component.
  ///
  /// Returns `None` if the component is a wildcard.
  auto component() -> std::optional<std::uint_fast64_t>;

  /// Parse a single numeric.
  auto numeric() -> std::optional<std::uint_fast64_t>;

  /// Optionally parse a dot, then a component.
  ///
  /// The second component of the tuple indicates if a wildcard has been
  /// encountered, and is always `false` if the first component is `Some`.
  ///
  /// If a dot is not encountered, `(None, false)` is returned.
  ///
  /// If a wildcard is encountered, `(None, true)` is returned.
  auto dot_component() -> std::optional<std::uint_fast64_t>;

  /// Parse a dot, then a numeric.
  auto dot_numeric() -> std::optional<std::uint_fast64_t>;

  /// Parse an string identifier.
  ///
  /// Like, `foo`, or `bar`.
  auto identifier() -> std::optional<Identifier>;

  /// Parse all pre-release identifiers, separated by dots.
  ///
  /// Like, `abcdef.1234`.
  auto pre() -> std::vector<Identifier>;

  /// Parse a dot-separated set of identifiers.
  auto parts() -> std::vector<Identifier>;

  /// Parse optional build metadata.
  ///
  /// Like, `` (empty), or `+abcdef`.
  auto plus_build_metadata() -> std::vector<Identifier>;

  /// Optionally parse a single operator.
  ///
  /// Like, `~`, or `^`.
  auto op() -> Op;

  /// Parse a single predicate.
  ///
  /// Like, `^1`, or `>=2.0.0`.
  auto predicate() -> std::optional<Predicate>;

  /// Parse a single range.
  ///
  /// Like, `^1.0` or `>=3.0.0, <4.0.0`.
  auto range() -> VersionReq;

  /// Parse a comparator.
  ///
  /// Like, `1.0 || 2.0` or `^1 || >=3.0.0, <4.0.0`.
  auto comparator() -> Comparator;

  /// Parse a version.
  ///
  /// Like, `1.0.0` or `3.0.0-beta.1`.
  auto version() -> Version;

  /// Check if we have reached the end of input.
  [[nodiscard]] inline auto is_eof() const -> bool {
    return lexer.size() < lexer.c1_index;
  }

  /// Get the rest of the tokens in the parser.
  ///
  /// Useful for debugging.
  auto tail() -> std::vector<Token>;

private:
  auto has_ws_separator(const Token::Kind& pat) -> bool;
};

auto parse(std::string_view input) -> Version;

//  namespace range {
//      VersionReq
//      parse(std::string_view input) {
//      }
//  } // end namespace range

} // end namespace semver
