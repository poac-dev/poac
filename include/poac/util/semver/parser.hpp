#ifndef POAC_UTIL_SEMVER_PARSER_HPP_
#define POAC_UTIL_SEMVER_PARSER_HPP_

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
  using string_type = std::string_view;
  using value_type = string_type::value_type;

  Lexer lexer;
  Token c1;

  /// Construct a new parser for the given input.
  explicit Parser(string_type str) : lexer(str), c1(lexer.next()) {}

  /// Pop one token.
  inline Token
  pop() {
    Token c1_ = this->c1;
    this->c1 = lexer.next();
    return c1_;
  }

  /// Peek one token.
  inline Token
  peek() const noexcept {
    return this->c1;
  }

  /// Skip whitespace if present.
  void
  skip_whitespace();

  /// Parse an optional comma separator, then if that is present a predicate.
  std::optional<Predicate>
  comma_predicate();

  /// Parse an optional or separator `||`, then if that is present a range.
  std::optional<VersionReq>
  or_range();

  /// Parse a single component.
  ///
  /// Returns `None` if the component is a wildcard.
  std::optional<std::uint_fast64_t>
  component();

  /// Parse a single numeric.
  std::optional<std::uint_fast64_t>
  numeric();

  /// Optionally parse a dot, then a component.
  ///
  /// The second component of the tuple indicates if a wildcard has been
  /// encountered, and is always `false` if the first component is `Some`.
  ///
  /// If a dot is not encountered, `(None, false)` is returned.
  ///
  /// If a wildcard is encountered, `(None, true)` is returned.
  std::optional<std::uint_fast64_t>
  dot_component();

  /// Parse a dot, then a numeric.
  std::optional<std::uint_fast64_t>
  dot_numeric();

  /// Parse an string identifier.
  ///
  /// Like, `foo`, or `bar`.
  std::optional<Identifier>
  identifier();

  /// Parse all pre-release identifiers, separated by dots.
  ///
  /// Like, `abcdef.1234`.
  std::vector<Identifier>
  pre();

  /// Parse a dot-separated set of identifiers.
  std::vector<Identifier>
  parts();

  /// Parse optional build metadata.
  ///
  /// Like, `` (empty), or `+abcdef`.
  std::vector<Identifier>
  plus_build_metadata();

  /// Optionally parse a single operator.
  ///
  /// Like, `~`, or `^`.
  Op
  op();

  /// Parse a single predicate.
  ///
  /// Like, `^1`, or `>=2.0.0`.
  std::optional<Predicate>
  predicate();

  /// Parse a single range.
  ///
  /// Like, `^1.0` or `>=3.0.0, <4.0.0`.
  VersionReq
  range();

  /// Parse a comparator.
  ///
  /// Like, `1.0 || 2.0` or `^1 || >=3.0.0, <4.0.0`.
  Comparator
  comparator();

  /// Parse a version.
  ///
  /// Like, `1.0.0` or `3.0.0-beta.1`.
  Version
  version();

  /// Check if we have reached the end of input.
  inline bool
  is_eof() const {
    return lexer.size() < lexer.c1_index;
  }

  /// Get the rest of the tokens in the parser.
  ///
  /// Useful for debugging.
  std::vector<Token>
  tail();

private:
  bool
  has_ws_separator(const Token::Kind& pat);
};

Version
parse(std::string_view input);

//  namespace range {
//      VersionReq
//      parse(std::string_view input) {
//      }
//  } // end namespace range

} // end namespace semver

#endif // POAC_UTIL_SEMVER_PARSER_HPP_
