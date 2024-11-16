#include "Semver.hpp"

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

std::ostream&
operator<<(std::ostream& os, const VersionToken& tok) noexcept {
  switch (tok.mKind) {
    case VersionToken::Num:
      os << std::get<uint64_t>(tok.mValue);
      break;
    case VersionToken::Ident:
      os << std::get<std::string_view>(tok.mValue);
      break;
    case VersionToken::Dot:
      os << '.';
      break;
    case VersionToken::Hyphen:
      os << '-';
      break;
    case VersionToken::Plus:
      os << '+';
      break;
    case VersionToken::Eof:
    case VersionToken::Unknown:
      break;
  }
  return os;
}

std::string
VersionToken::toString() const noexcept {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

size_t
VersionToken::size() const noexcept {
  return toString().size();
}

bool
operator==(const VersionToken& lhs, const VersionToken& rhs) noexcept {
  if (lhs.mKind != rhs.mKind) {
    return false;
  }
  switch (lhs.mKind) {
    case VersionToken::Num:
      return std::get<uint64_t>(lhs.mValue) == std::get<uint64_t>(rhs.mValue);
    case VersionToken::Ident:
      return std::get<std::string_view>(lhs.mValue)
             == std::get<std::string_view>(rhs.mValue);
    case VersionToken::Dot:
    case VersionToken::Hyphen:
    case VersionToken::Plus:
    case VersionToken::Eof:
    case VersionToken::Unknown:
      return true;
  }
  return false;
}

bool
operator<(const VersionToken& lhs, const VersionToken& rhs) noexcept {
  if (lhs.mKind == VersionToken::Num && rhs.mKind == VersionToken::Num) {
    return std::get<uint64_t>(lhs.mValue) < std::get<uint64_t>(rhs.mValue);
  }
  return lhs.toString() < rhs.toString();
}
bool
operator>(const VersionToken& lhs, const VersionToken& rhs) {
  return rhs < lhs;
}

static std::string
carets(const VersionToken& tok) noexcept {
  switch (tok.mKind) {
    case VersionToken::Eof:
    case VersionToken::Unknown:
      return "^";
    default:
      // NOLINTNEXTLINE(modernize-return-braced-init-list)
      return std::string(tok.size(), '^');
  }
}

bool
Prerelease::empty() const noexcept {
  return mIdent.empty();
}

std::string
Prerelease::toString() const noexcept {
  std::string str;
  for (size_t i = 0; i < mIdent.size(); ++i) {
    if (i > 0) {
      str += '.';
    }
    str += mIdent[i].toString();
  }
  return str;
}

bool
operator==(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  return lhs.mIdent == rhs.mIdent;
}
bool
operator!=(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  return !(lhs == rhs);
}
bool
operator<(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  if (lhs.mIdent.empty()) {
    return false; // lhs is a normal version and is greater
  }
  if (rhs.mIdent.empty()) {
    return true; // rhs is a normal version and is greater
  }
  for (size_t i = 0; i < lhs.mIdent.size() && i < rhs.mIdent.size(); ++i) {
    if (lhs.mIdent[i] < rhs.mIdent[i]) {
      return true;
    } else if (lhs.mIdent[i] > rhs.mIdent[i]) {
      return false;
    }
  }
  return lhs.mIdent.size() < rhs.mIdent.size();
}
bool
operator>(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  return rhs < lhs;
}
bool
operator<=(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  return !(rhs < lhs);
}
bool
operator>=(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  return !(lhs < rhs);
}

bool
BuildMetadata::empty() const noexcept {
  return mIdent.empty();
}

std::string
BuildMetadata::toString() const noexcept {
  std::string str;
  for (size_t i = 0; i < mIdent.size(); ++i) {
    if (i > 0) {
      str += '.';
    }
    str += mIdent[i].toString();
  }
  return str;
}

bool
operator==(const BuildMetadata& lhs, const BuildMetadata& rhs) noexcept {
  return lhs.mIdent == rhs.mIdent;
}
bool
operator<(const BuildMetadata& lhs, const BuildMetadata& rhs) noexcept {
  for (size_t i = 0; i < lhs.mIdent.size() && i < rhs.mIdent.size(); ++i) {
    if (lhs.mIdent[i] < rhs.mIdent[i]) {
      return true;
    } else if (lhs.mIdent[i] > rhs.mIdent[i]) {
      return false;
    }
  }
  return lhs.mIdent.size() < rhs.mIdent.size();
}
bool
operator>(const BuildMetadata& lhs, const BuildMetadata& rhs) noexcept {
  return rhs < lhs;
}

std::string
Version::toString() const noexcept {
  std::string str = std::to_string(mMajor);
  str += '.' + std::to_string(mMinor);
  str += '.' + std::to_string(mPatch);
  if (!mPre.empty()) {
    str += '-';
    str += mPre.toString();
  }
  if (!mBuild.empty()) {
    str += '+';
    str += mBuild.toString();
  }
  return str;
}

std::ostream&
operator<<(std::ostream& os, const Version& ver) noexcept {
  os << ver.toString();
  return os;
}

bool
operator==(const Version& lhs, const Version& rhs) noexcept {
  return lhs.mMajor == rhs.mMajor && lhs.mMinor == rhs.mMinor
         && lhs.mPatch == rhs.mPatch && lhs.mPre == rhs.mPre
         && lhs.mBuild == rhs.mBuild;
}
bool
operator!=(const Version& lhs, const Version& rhs) noexcept {
  return !(lhs == rhs);
}

bool
operator<(const Version& lhs, const Version& rhs) noexcept {
  if (lhs.mMajor < rhs.mMajor) {
    return true;
  } else if (lhs.mMajor > rhs.mMajor) {
    return false;
  } else if (lhs.mMinor < rhs.mMinor) {
    return true;
  } else if (lhs.mMinor > rhs.mMinor) {
    return false;
  } else if (lhs.mPatch < rhs.mPatch) {
    return true;
  } else if (lhs.mPatch > rhs.mPatch) {
    return false;
  } else if (!lhs.mPre.empty() && rhs.mPre.empty()) {
    // lhs has a pre-release tag and rhs doesn't, so lhs < rhs
    return true;
  } else if (lhs.mPre.empty() && !rhs.mPre.empty()) {
    // lhs doesn't have a pre-release tag and rhs does, so lhs > rhs
    return false;
  } else if (lhs.mPre < rhs.mPre) {
    // Both lhs and rhs have pre-release tags, so compare them
    return true;
  } else if (lhs.mPre > rhs.mPre) {
    return false;
  } else if (lhs.mBuild < rhs.mBuild) {
    return true;
  } else if (lhs.mBuild > rhs.mBuild) {
    return false;
  } else {
    return false;
  }
}
bool
operator>(const Version& lhs, const Version& rhs) noexcept {
  return rhs < lhs;
}
bool
operator<=(const Version& lhs, const Version& rhs) noexcept {
  return !(rhs < lhs);
}
bool
operator>=(const Version& lhs, const Version& rhs) noexcept {
  return !(lhs < rhs);
}

VersionToken
VersionLexer::consumeIdent() noexcept {
  size_t len = 0;
  while (mPos < mS.size() && (std::isalnum(mS[mPos]) || mS[mPos] == '-')) {
    step();
    ++len;
  }
  return { VersionToken::Ident, std::string_view(mS.data() + mPos - len, len) };
}

VersionToken
VersionLexer::consumeNum() {
  size_t len = 0;
  uint64_t value = 0;
  while (mPos < mS.size() && std::isdigit(mS[mPos])) {
    if (len > 0 && value == 0) {
      throw SemverError(
          mS, '\n', std::string(mPos - len, ' '), "^ invalid leading zero"
      );
    }

    const uint64_t digit = mS[mPos] - '0';
    constexpr uint64_t base = 10;
    // Check for overflow
    if (value > (std::numeric_limits<uint64_t>::max() - digit) / base) {
      throw SemverError(
          mS, '\n', std::string(mPos - len, ' '), std::string(len, '^'),
          " number exceeds UINT64_MAX"
      );
    }

    value = value * base + digit;
    step();
    ++len;
  }
  return { VersionToken::Num, value };
}

// Note that 012 is an invalid number but 012d is a valid identifier.
VersionToken
VersionLexer::consumeNumOrIdent() {
  const size_t oldPos = mPos; // we need two passes
  bool isIdent = false;
  while (mPos < mS.size() && (std::isalnum(mS[mPos]) || mS[mPos] == '-')) {
    if (!std::isdigit(mS[mPos])) {
      isIdent = true;
    }
    step();
  }

  mPos = oldPos;
  if (isIdent) {
    return consumeIdent();
  } else {
    return consumeNum();
  }
}

VersionToken
VersionLexer::next() {
  if (isEof()) {
    return VersionToken{ VersionToken::Eof };
  }

  const char c = mS[mPos];
  if (std::isalpha(c)) {
    return consumeIdent();
  } else if (std::isdigit(c)) {
    return consumeNumOrIdent();
  } else if (c == '.') {
    step();
    return VersionToken{ VersionToken::Dot };
  } else if (c == '-') {
    step();
    return VersionToken{ VersionToken::Hyphen };
  } else if (c == '+') {
    step();
    return VersionToken{ VersionToken::Plus };
  } else {
    step();
    return VersionToken{ VersionToken::Unknown };
  }
}

VersionToken
VersionLexer::peek() {
  const size_t oldPos = mPos;
  const VersionToken tok = next();
  mPos = oldPos;
  return tok;
}

struct SemverParseError : public SemverError {
  SemverParseError(
      const VersionLexer& lexer, const VersionToken& tok,
      const std::string_view msg
  )
      : SemverError(
            lexer.mS, '\n', std::string(lexer.mPos, ' '), carets(tok), msg
        ) {}
};

Version
VersionParser::parse() {
  if (mLexer.peek().mKind == VersionToken::Eof) {
    throw SemverError("empty string is not a valid semver");
  }

  Version ver;
  ver.mMajor = parseNum();
  parseDot();
  ver.mMinor = parseNum();
  parseDot();
  ver.mPatch = parseNum();

  if (mLexer.peek().mKind == VersionToken::Hyphen) {
    mLexer.step();
    ver.mPre = parsePre();
  } else {
    ver.mPre = Prerelease();
  }

  if (mLexer.peek().mKind == VersionToken::Plus) {
    mLexer.step();
    ver.mBuild = parseBuild();
  } else {
    ver.mBuild = BuildMetadata();
  }

  if (!mLexer.isEof()) {
    throw SemverParseError(
        mLexer, mLexer.peek(),
        " unexpected character: `" + std::string(1, mLexer.mS[mLexer.mPos])
            + '`'
    );
  }

  return ver;
}

// Even if the token can be parsed as an identifier, try to parse it as a
// number.
uint64_t
VersionParser::parseNum() {
  if (!std::isdigit(mLexer.mS[mLexer.mPos])) {
    throw SemverParseError(mLexer, mLexer.peek(), " expected number");
  }
  return std::get<uint64_t>(mLexer.consumeNum().mValue);
}

void
VersionParser::parseDot() {
  const VersionToken tok = mLexer.next();
  if (tok.mKind != VersionToken::Dot) {
    throw SemverParseError(mLexer, tok, " expected `.`");
  }
}

// pre ::= numOrIdent ("." numOrIdent)*
Prerelease
VersionParser::parsePre() {
  std::vector<VersionToken> pre;
  pre.emplace_back(parseNumOrIdent());
  while (mLexer.peek().mKind == VersionToken::Dot) {
    mLexer.step();
    pre.emplace_back(parseNumOrIdent());
  }
  return Prerelease{ pre };
}

// numOrIdent ::= num | ident
VersionToken
VersionParser::parseNumOrIdent() {
  const VersionToken tok = mLexer.next();
  if (tok.mKind != VersionToken::Num && tok.mKind != VersionToken::Ident) {
    throw SemverParseError(mLexer, tok, " expected number or identifier");
  }
  return tok;
}

// build ::= ident ("." ident)*
BuildMetadata
VersionParser::parseBuild() {
  std::vector<VersionToken> build;
  build.emplace_back(parseIdent());
  while (mLexer.peek().mKind == VersionToken::Dot) {
    mLexer.step();
    build.emplace_back(parseIdent());
  }
  return BuildMetadata{ build };
}

// Even if the token can be parsed as a number, try to parse it as an
// identifier.
VersionToken
VersionParser::parseIdent() {
  if (!std::isalnum(mLexer.mS[mLexer.mPos])) {
    throw SemverParseError(mLexer, mLexer.peek(), " expected identifier");
  }
  return mLexer.consumeIdent();
}

Prerelease
Prerelease::parse(const std::string_view str) {
  VersionParser parser(str);
  return parser.parsePre();
}

BuildMetadata
BuildMetadata::parse(const std::string_view str) {
  VersionParser parser(str);
  return parser.parseBuild();
}

Version
Version::parse(const std::string_view str) {
  VersionParser parser(str);
  return parser.parse();
}

#ifdef POAC_TEST

#  include "Rustify/Tests.hpp"

namespace tests {

// Thanks to:
// https://github.com/dtolnay/semver/blob/55fa2cadd6ec95be02e5a2a87b24355304e44d40/tests/test_version.rs#L13

static void
testParse() {
  assertException<SemverError>(
      []() { Version::parse(""); },
      "invalid semver:\n"
      "empty string is not a valid semver"
  );
  assertException<SemverError>(
      []() { Version::parse("  "); },
      "invalid semver:\n"
      "  \n"
      "^ expected number"
  );
  assertException<SemverError>(
      []() { Version::parse("1"); },
      "invalid semver:\n"
      "1\n"
      " ^ expected `.`"
  );
  assertException<SemverError>(
      []() { Version::parse("1.2"); },
      "invalid semver:\n"
      "1.2\n"
      "   ^ expected `.`"
  );
  assertException<SemverError>(
      []() { Version::parse("1.2.3-"); },
      "invalid semver:\n"
      "1.2.3-\n"
      "      ^ expected number or identifier"
  );
  assertException<SemverError>(
      []() { Version::parse("00"); },
      "invalid semver:\n"
      "00\n"
      "^ invalid leading zero"
  );
  assertException<SemverError>(
      []() { Version::parse("0.00.0"); },
      "invalid semver:\n"
      "0.00.0\n"
      "  ^ invalid leading zero"
  );
  assertException<SemverError>(
      []() { Version::parse("0.0.0.0"); },
      "invalid semver:\n"
      "0.0.0.0\n"
      "     ^ unexpected character: `.`"
  );
  assertException<SemverError>(
      []() { Version::parse("a.b.c"); },
      "invalid semver:\n"
      "a.b.c\n"
      "^ expected number"
  );
  assertException<SemverError>(
      []() { Version::parse("1.2.3 abc"); },
      "invalid semver:\n"
      "1.2.3 abc\n"
      "     ^ unexpected character: ` `"
  );
  assertException<SemverError>(
      []() { Version::parse("1.2.3-01"); },
      "invalid semver:\n"
      "1.2.3-01\n"
      "      ^ invalid leading zero"
  );
  assertException<SemverError>(
      []() { Version::parse("1.2.3++"); },
      "invalid semver:\n"
      "1.2.3++\n"
      "      ^ expected identifier"
  );
  assertException<SemverError>(
      []() { Version::parse("07"); },
      "invalid semver:\n"
      "07\n"
      "^ invalid leading zero"
  );
  assertException<SemverError>(
      []() { Version::parse("111111111111111111111.0.0"); },
      "invalid semver:\n"
      "111111111111111111111.0.0\n"
      "^^^^^^^^^^^^^^^^^^^^ number exceeds UINT64_MAX"
  );
  assertException<SemverError>(
      []() { Version::parse("0.99999999999999999999999.0"); },
      "invalid semver:\n"
      "0.99999999999999999999999.0\n"
      "  ^^^^^^^^^^^^^^^^^^^ number exceeds UINT64_MAX"
  );
  assertException<SemverError>(
      []() { Version::parse("8\0"); },
      "invalid semver:\n"
      "8\n"
      " ^ expected `.`"
  );

  assertEq(
      Version::parse("1.2.3"), (Version{ .mMajor = 1,
                                         .mMinor = 2,
                                         .mPatch = 3,
                                         .mPre = Prerelease(),
                                         .mBuild = BuildMetadata() })
  );
  assertEq(
      Version::parse("1.2.3-alpha1"),
      (Version{ .mMajor = 1,
                .mMinor = 2,
                .mPatch = 3,
                .mPre = Prerelease::parse("alpha1"),
                .mBuild = BuildMetadata() })
  );
  assertEq(
      Version::parse("1.2.3+build5"),
      (Version{ .mMajor = 1,
                .mMinor = 2,
                .mPatch = 3,
                .mPre = Prerelease(),
                .mBuild = BuildMetadata::parse("build5") })
  );
  assertEq(
      Version::parse("1.2.3+5build"),
      (Version{ .mMajor = 1,
                .mMinor = 2,
                .mPatch = 3,
                .mPre = Prerelease(),
                .mBuild = BuildMetadata::parse("5build") })
  );
  assertEq(
      Version::parse("1.2.3-alpha1+build5"),
      (Version{ .mMajor = 1,
                .mMinor = 2,
                .mPatch = 3,
                .mPre = Prerelease::parse("alpha1"),
                .mBuild = BuildMetadata::parse("build5") })
  );
  assertEq(
      Version::parse("1.2.3-1.alpha1.9+build5.7.3aedf"),
      (Version{ .mMajor = 1,
                .mMinor = 2,
                .mPatch = 3,
                .mPre = Prerelease::parse("1.alpha1.9"),
                .mBuild = BuildMetadata::parse("build5.7.3aedf") })
  );
  assertEq(
      Version::parse("1.2.3-0a.alpha1.9+05build.7.3aedf"),
      (Version{ .mMajor = 1,
                .mMinor = 2,
                .mPatch = 3,
                .mPre = Prerelease::parse("0a.alpha1.9"),
                .mBuild = BuildMetadata::parse("05build.7.3aedf") })
  );
  assertEq(
      Version::parse("0.4.0-beta.1+0851523"),
      (Version{ .mMajor = 0,
                .mMinor = 4,
                .mPatch = 0,
                .mPre = Prerelease::parse("beta.1"),
                .mBuild = BuildMetadata::parse("0851523") })
  );
  assertEq(
      Version::parse("1.1.0-beta-10"),
      (Version{ .mMajor = 1,
                .mMinor = 1,
                .mPatch = 0,
                .mPre = Prerelease::parse("beta-10"),
                .mBuild = BuildMetadata() })
  );

  pass();
}

static void
testEq() {
  assertEq(Version::parse("1.2.3"), Version::parse("1.2.3"));
  assertEq(Version::parse("1.2.3-alpha1"), Version::parse("1.2.3-alpha1"));
  assertEq(Version::parse("1.2.3+build.42"), Version::parse("1.2.3+build.42"));
  assertEq(
      Version::parse("1.2.3-alpha1+42"), Version::parse("1.2.3-alpha1+42")
  );

  pass();
}

static void
testNe() {
  assertNe(Version::parse("0.0.0"), Version::parse("0.0.1"));
  assertNe(Version::parse("0.0.0"), Version::parse("0.1.0"));
  assertNe(Version::parse("0.0.0"), Version::parse("1.0.0"));
  assertNe(Version::parse("1.2.3-alpha"), Version::parse("1.2.3-beta"));
  assertNe(Version::parse("1.2.3+23"), Version::parse("1.2.3+42"));

  pass();
}

static void
testDisplay() {
  {
    std::ostringstream oss;
    oss << Version::parse("1.2.3");
    assertEq(oss.str(), "1.2.3");
  }
  {
    std::ostringstream oss;
    oss << Version::parse("1.2.3-alpha1");
    assertEq(oss.str(), "1.2.3-alpha1");
  }
  {
    std::ostringstream oss;
    oss << Version::parse("1.2.3+build.42");
    assertEq(oss.str(), "1.2.3+build.42");
  }
  {
    std::ostringstream oss;
    oss << Version::parse("1.2.3-alpha1+42");
    assertEq(oss.str(), "1.2.3-alpha1+42");
  }

  pass();
}

static void
testLt() {
  assertLt(Version::parse("0.0.0"), Version::parse("1.2.3-alpha2"));
  assertLt(Version::parse("1.0.0"), Version::parse("1.2.3-alpha2"));
  assertLt(Version::parse("1.2.0"), Version::parse("1.2.3-alpha2"));
  assertLt(Version::parse("1.2.3-alpha1"), Version::parse("1.2.3"));
  assertLt(Version::parse("1.2.3-alpha1"), Version::parse("1.2.3-alpha2"));
  assertFalse(Version::parse("1.2.3-alpha2") < Version::parse("1.2.3-alpha2"));
  assertLt(Version::parse("1.2.3+23"), Version::parse("1.2.3+42"));

  pass();
}

static void
testLe() {
  assertTrue(Version::parse("0.0.0") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.0.0") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.2.0") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.2.3-alpha1") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.2.3-alpha2") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.2.3+23") <= Version::parse("1.2.3+42"));

  pass();
}

static void
testGt() {
  assertTrue(Version::parse("1.2.3-alpha2") > Version::parse("0.0.0"));
  assertTrue(Version::parse("1.2.3-alpha2") > Version::parse("1.0.0"));
  assertTrue(Version::parse("1.2.3-alpha2") > Version::parse("1.2.0"));
  assertTrue(Version::parse("1.2.3-alpha2") > Version::parse("1.2.3-alpha1"));
  assertTrue(Version::parse("1.2.3") > Version::parse("1.2.3-alpha2"));
  assertFalse(Version::parse("1.2.3-alpha2") > Version::parse("1.2.3-alpha2"));
  assertFalse(Version::parse("1.2.3+23") > Version::parse("1.2.3+42"));

  pass();
}

static void
testGe() {
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("0.0.0"));
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("1.0.0"));
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("1.2.0"));
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("1.2.3-alpha1"));
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("1.2.3-alpha2"));
  assertFalse(Version::parse("1.2.3+23") >= Version::parse("1.2.3+42"));

  pass();
}

static void
testSpecOrder() {
  const std::vector<std::string> vers = {
    "1.0.0-alpha",  "1.0.0-alpha.1", "1.0.0-alpha.beta", "1.0.0-beta",
    "1.0.0-beta.2", "1.0.0-beta.11", "1.0.0-rc.1",       "1.0.0",
  };
  for (size_t i = 1; i < vers.size(); ++i) {
    assertLt(Version::parse(vers[i - 1]), Version::parse(vers[i]));
  }

  pass();
}

} // namespace tests

int
main() {
  tests::testParse();
  tests::testEq();
  tests::testNe();
  tests::testDisplay();
  tests::testLt();
  tests::testLe();
  tests::testGt();
  tests::testGe();
  tests::testSpecOrder();
}

#endif
