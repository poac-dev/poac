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

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

struct SemverError : public PoacError {
  explicit SemverError(auto&&... args)
      : PoacError("invalid semver:\n", std::forward<decltype(args)>(args)...) {}
};

struct VersionToken {
  enum class Kind : uint8_t {
    Num, // [1-9][0-9]*
    Ident, // [a-zA-Z0-9][a-zA-Z0-9-]*
    Dot, // .
    Hyphen, // -
    Plus, // +
    Eof,
    Unknown,
  };
  using enum Kind;

  Kind mKind;
  std::variant<std::monostate, uint64_t, std::string_view> mValue;

  constexpr VersionToken(
      Kind kind,
      const std::variant<std::monostate, uint64_t, std::string_view>& value
  ) noexcept
      : mKind(kind), mValue(value) {}
  constexpr explicit VersionToken(Kind kind) noexcept
      : mKind(kind), mValue(std::monostate{}) {}

  std::string toString() const noexcept;
  size_t size() const noexcept;
};

struct Prerelease {
  std::vector<VersionToken> mIdent;

  static Prerelease parse(std::string_view str);
  bool empty() const noexcept;
  std::string toString() const noexcept;
};
bool operator==(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator!=(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator<(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator>(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator<=(const Prerelease& lhs, const Prerelease& rhs) noexcept;
bool operator>=(const Prerelease& lhs, const Prerelease& rhs) noexcept;

struct BuildMetadata {
  std::vector<VersionToken> mIdent;

  static BuildMetadata parse(std::string_view str);
  bool empty() const noexcept;
  std::string toString() const noexcept;
};

struct Version {
  uint64_t mMajor{};
  uint64_t mMinor{};
  uint64_t mPatch{};
  Prerelease mPre;
  BuildMetadata mBuild;

  static Version parse(std::string_view str);
  std::string toString() const noexcept;
};
std::ostream& operator<<(std::ostream& os, const Version& ver) noexcept;
bool operator==(const Version& lhs, const Version& rhs) noexcept;
bool operator!=(const Version& lhs, const Version& rhs) noexcept;
bool operator<(const Version& lhs, const Version& rhs) noexcept;
bool operator>(const Version& lhs, const Version& rhs) noexcept;
bool operator<=(const Version& lhs, const Version& rhs) noexcept;
bool operator>=(const Version& lhs, const Version& rhs) noexcept;

struct VersionLexer {
  std::string_view mS;
  size_t mPos{ 0 };

  constexpr explicit VersionLexer(const std::string_view str) noexcept
      : mS(str) {}

  constexpr bool isEof() const noexcept {
    return mPos >= mS.size();
  }
  constexpr void step() noexcept {
    ++mPos;
  }
  VersionToken consumeIdent() noexcept;
  VersionToken consumeNum();
  VersionToken consumeNumOrIdent();
  VersionToken next();
  VersionToken peek();
};

struct VersionParser {
  VersionLexer mLexer;

  constexpr explicit VersionParser(const std::string_view str) noexcept
      : mLexer(str) {}

  Version parse();
  uint64_t parseNum();
  void parseDot();
  Prerelease parsePre();
  VersionToken parseNumOrIdent();
  BuildMetadata parseBuild();
  VersionToken parseIdent();
};
