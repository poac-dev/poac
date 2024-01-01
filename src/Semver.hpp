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

#include "Rustify.hpp"

#include <ostream>
#include <utility>
#include <variant>

struct VersionToken {
  enum Kind {
    Num, // [1-9][0-9]*
    Ident, // [a-zA-Z0-9][a-zA-Z0-9-]*
    Dot, // .
    Hyphen, // -
    Plus, // +
    Eof,
    Unknown,
  };

  Kind kind;
  std::variant<std::monostate, u64, StringRef> value;

  VersionToken(
      Kind kind, std::variant<std::monostate, u64, StringRef> value
  ) noexcept
      : kind(kind), value(std::move(value)) {}
  explicit VersionToken(Kind kind) noexcept
      : kind(kind), value(std::monostate{}) {}

  String to_string() const noexcept;
  usize size() const noexcept;
};

struct Prerelease {
  Vec<VersionToken> ident;

  static Prerelease parse(StringRef);
  bool empty() const noexcept;
  String to_string() const noexcept;
};
bool operator==(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator!=(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator<(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator>(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator<=(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator>=(const Prerelease& lhs, const Prerelease& rhs) noexcept;

struct BuildMetadata {
  Vec<VersionToken> ident;

  static BuildMetadata parse(StringRef);
  bool empty() const noexcept;
  String to_string() const noexcept;
};

struct Version {
  u64 major;
  u64 minor;
  u64 patch;
  Prerelease pre;
  BuildMetadata build;

  static Version parse(StringRef);
  String to_string() const noexcept;
};
std::ostream& operator<<(std::ostream&, const Version&) noexcept;
bool operator==(const Version&, const Version&) noexcept;
bool operator!=(const Version&, const Version&) noexcept;
bool operator<(const Version&, const Version&) noexcept;
bool operator>(const Version&, const Version&) noexcept;
bool operator<=(const Version&, const Version&) noexcept;
bool operator>=(const Version&, const Version&) noexcept;

struct VersionLexer {
  StringRef s;
  usize pos;

  explicit VersionLexer(StringRef s) : s(s), pos(0) {}

  bool isEof() const noexcept;
  void step() noexcept;
  VersionToken consumeIdent() noexcept;
  VersionToken consumeNum();
  VersionToken consumeNumOrIdent();
  VersionToken next();
  VersionToken peek();
};

struct VersionParser {
  VersionLexer lexer;

  explicit VersionParser(StringRef s) : lexer(s) {}

  Version parse();
  u64 parseNum();
  void parseDot();
  Prerelease parsePre();
  VersionToken parseNumOrIdent();
  BuildMetadata parseBuild();
  VersionToken parseIdent();
};
