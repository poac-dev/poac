#pragma once

#include "Rustify.hpp"

#include <ostream>
#include <utility>
#include <variant>

struct Token {
  enum Kind {
    Num, // [1-9][0-9]*
    Ident, // [a-zA-Z0-9][a-zA-Z0-9-]*
    Dot, // .
    Hyphen, // -
    Plus, // +
    Eof,
  };

  Kind kind;
  std::variant<std::monostate, u64, StringRef> value;

  Token(Kind kind, std::variant<std::monostate, u64, StringRef> value)
      : kind(kind), value(std::move(value)) {}
  explicit Token(Kind kind) : kind(kind), value(std::monostate{}) {}

  String to_string() const;
  usize size() const;
};

struct Prerelease {
  Vec<Token> ident;

  static Prerelease parse(StringRef);
  bool empty() const;
  String to_string() const;
};

struct BuildMetadata {
  Vec<Token> ident;

  static BuildMetadata parse(StringRef);
  bool empty() const;
  String to_string() const;
};

struct Version {
  u64 major;
  u64 minor;
  u64 patch;
  Prerelease pre;
  BuildMetadata build;

  String to_string() const;
};
std::ostream& operator<<(std::ostream&, const Version&);
bool operator==(const Version&, const Version&);
bool operator!=(const Version&, const Version&);
bool operator<(const Version&, const Version&);
bool operator>(const Version&, const Version&);
bool operator<=(const Version&, const Version&);
bool operator>=(const Version&, const Version&);

Version parseSemver(StringRef);
