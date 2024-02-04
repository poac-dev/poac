// Semver parser
//
// Syntax:
//   version    ::= num "." num "." num ("-" pre)? ("+" build)?
//   pre        ::= numOrIdent ("." numOrIdent)*
//   build      ::= ident ("." ident)*
//   numOrIdent ::= num | ident
//   num        ::= [1-9][0-9]*
//   ident      ::= [a-zA-Z0-9][a-zA-Z0-9-]*
#pragma once

#include "Exception.hpp"
#include "Rustify.hpp"

#include <ostream>
#include <utility>
#include <variant>

struct SemverError : public PoacError {
  explicit SemverError(auto&&... args)
      : PoacError("invalid semver:\n", std::forward<decltype(args)>(args)...) {}
};

struct VersionToken {
  enum class Kind {
    Num, // [1-9][0-9]*
    Ident, // [a-zA-Z0-9][a-zA-Z0-9-]*
    Dot, // .
    Hyphen, // -
    Plus, // +
    Eof,
    Unknown,
  };
  using enum Kind;

  Kind kind;
  std::variant<std::monostate, u64, StringRef> value;

  constexpr VersionToken(
      Kind kind, const std::variant<std::monostate, u64, StringRef>& value
  ) noexcept
      : kind(kind), value(value) {}
  constexpr explicit VersionToken(Kind kind) noexcept
      : kind(kind), value(std::monostate{}) {}

  String toString() const noexcept;
  usize size() const noexcept;
};

struct Prerelease {
  Vec<VersionToken> ident;

  static Prerelease parse(StringRef str);
  bool empty() const noexcept;
  String toString() const noexcept;
};
bool operator==(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator!=(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator<(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator>(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator<=(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator>=(const Prerelease& lhs, const Prerelease& rhs) noexcept;

struct BuildMetadata {
  Vec<VersionToken> ident;

  static BuildMetadata parse(StringRef str);
  bool empty() const noexcept;
  String toString() const noexcept;
};

struct Version {
  u64 major{};
  u64 minor{};
  u64 patch{};
  Prerelease pre;
  BuildMetadata build;

  static Version parse(StringRef str);
  String toString() const noexcept;
};
std::ostream& operator<<(std::ostream& os, const Version& ver) noexcept;
bool operator==(const Version& lhs, const Version& rhs) noexcept;
bool operator!=(const Version& lhs, const Version& rhs) noexcept;
bool operator<(const Version& lhs, const Version& rhs) noexcept;
bool operator>(const Version& lhs, const Version& rhs) noexcept;
bool operator<=(const Version& lhs, const Version& rhs) noexcept;
bool operator>=(const Version& lhs, const Version& rhs) noexcept;

struct VersionLexer {
  StringRef s;
  usize pos{ 0 };

  constexpr explicit VersionLexer(const StringRef str) noexcept : s(str) {}

  constexpr bool isEof() const noexcept {
    return pos >= s.size();
  }
  constexpr void step() noexcept {
    ++pos;
  }
  VersionToken consumeIdent() noexcept;
  VersionToken consumeNum();
  VersionToken consumeNumOrIdent();
  VersionToken next();
  VersionToken peek();
};

struct VersionParser {
  VersionLexer lexer;

  constexpr explicit VersionParser(const StringRef str) noexcept : lexer(str) {}

  Version parse();
  u64 parseNum();
  void parseDot();
  Prerelease parsePre();
  VersionToken parseNumOrIdent();
  BuildMetadata parseBuild();
  VersionToken parseIdent();
};
