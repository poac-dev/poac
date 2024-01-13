#include "Semver.hpp"

#include "Rustify.hpp"

#include <cctype>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>

std::ostream&
operator<<(std::ostream& os, const VersionToken& tok) noexcept {
  switch (tok.kind) {
    case VersionToken::Num:
      os << std::get<u64>(tok.value);
      break;
    case VersionToken::Ident:
      os << std::get<StringRef>(tok.value);
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

String
VersionToken::toString() const noexcept {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

usize
VersionToken::size() const noexcept {
  return toString().size();
}

bool
operator==(const VersionToken& lhs, const VersionToken& rhs) noexcept {
  if (lhs.kind != rhs.kind) {
    return false;
  }
  switch (lhs.kind) {
    case VersionToken::Num:
      return std::get<u64>(lhs.value) == std::get<u64>(rhs.value);
    case VersionToken::Ident:
      return std::get<StringRef>(lhs.value) == std::get<StringRef>(rhs.value);
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
  if (lhs.kind == VersionToken::Num && rhs.kind == VersionToken::Num) {
    return std::get<u64>(lhs.value) < std::get<u64>(rhs.value);
  }
  return lhs.toString() < rhs.toString();
}
bool
operator>(const VersionToken& lhs, const VersionToken& rhs) {
  return rhs < lhs;
}

static String
carets(const VersionToken& tok) noexcept {
  switch (tok.kind) {
    case VersionToken::Eof:
    case VersionToken::Unknown:
      return "^";
    default:
      return String(tok.size(), '^');
  }
}

bool
Prerelease::empty() const noexcept {
  return ident.empty();
}

String
Prerelease::toString() const noexcept {
  String s;
  for (usize i = 0; i < ident.size(); ++i) {
    if (i > 0) {
      s += '.';
    }
    s += ident[i].toString();
  }
  return s;
}

bool
operator==(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  return lhs.ident == rhs.ident;
}
bool
operator!=(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  return !(lhs == rhs);
}
bool
operator<(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  if (lhs.ident.empty()) {
    return false; // lhs is a normal version and is greater
  }
  if (rhs.ident.empty()) {
    return true; // rhs is a normal version and is greater
  }
  for (usize i = 0; i < lhs.ident.size() && i < rhs.ident.size(); ++i) {
    if (lhs.ident[i] < rhs.ident[i]) {
      return true;
    } else if (lhs.ident[i] > rhs.ident[i]) {
      return false;
    }
  }
  return lhs.ident.size() < rhs.ident.size();
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
  return ident.empty();
}

String
BuildMetadata::toString() const noexcept {
  String s;
  for (usize i = 0; i < ident.size(); ++i) {
    if (i > 0) {
      s += '.';
    }
    s += ident[i].toString();
  }
  return s;
}

bool
operator==(const BuildMetadata& lhs, const BuildMetadata& rhs) noexcept {
  return lhs.ident == rhs.ident;
}
bool
operator<(const BuildMetadata& lhs, const BuildMetadata& rhs) noexcept {
  for (usize i = 0; i < lhs.ident.size() && i < rhs.ident.size(); ++i) {
    if (lhs.ident[i] < rhs.ident[i]) {
      return true;
    } else if (lhs.ident[i] > rhs.ident[i]) {
      return false;
    }
  }
  return lhs.ident.size() < rhs.ident.size();
}
bool
operator>(const BuildMetadata& lhs, const BuildMetadata& rhs) noexcept {
  return rhs < lhs;
}

String
Version::toString() const noexcept {
  String s = std::to_string(major);
  s += '.' + std::to_string(minor);
  s += '.' + std::to_string(patch);
  if (!pre.empty()) {
    s += '-';
    s += pre.toString();
  }
  if (!build.empty()) {
    s += '+';
    s += build.toString();
  }
  return s;
}

std::ostream&
operator<<(std::ostream& os, const Version& v) noexcept {
  os << v.toString();
  return os;
}

bool
operator==(const Version& lhs, const Version& rhs) noexcept {
  return lhs.major == rhs.major && lhs.minor == rhs.minor
         && lhs.patch == rhs.patch && lhs.pre == rhs.pre
         && lhs.build == rhs.build;
}
bool
operator!=(const Version& lhs, const Version& rhs) noexcept {
  return !(lhs == rhs);
}

bool
operator<(const Version& lhs, const Version& rhs) noexcept {
  if (lhs.major < rhs.major) {
    return true;
  } else if (lhs.major > rhs.major) {
    return false;
  } else if (lhs.minor < rhs.minor) {
    return true;
  } else if (lhs.minor > rhs.minor) {
    return false;
  } else if (lhs.patch < rhs.patch) {
    return true;
  } else if (lhs.patch > rhs.patch) {
    return false;
  } else if (!lhs.pre.empty() && rhs.pre.empty()) {
    // lhs has a pre-release tag and rhs doesn't, so lhs < rhs
    return true;
  } else if (lhs.pre.empty() && !rhs.pre.empty()) {
    // lhs doesn't have a pre-release tag and rhs does, so lhs > rhs
    return false;
  } else if (lhs.pre < rhs.pre) {
    // Both lhs and rhs have pre-release tags, so compare them
    return true;
  } else if (lhs.pre > rhs.pre) {
    return false;
  } else if (lhs.build < rhs.build) {
    return true;
  } else if (lhs.build > rhs.build) {
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

bool
VersionLexer::isEof() const noexcept {
  return pos >= s.size();
}

void
VersionLexer::step() noexcept {
  ++pos;
}

VersionToken
VersionLexer::consumeIdent() noexcept {
  usize len = 0;
  while (pos < s.size() && (std::isalnum(s[pos]) || s[pos] == '-')) {
    step();
    ++len;
  }
  return { VersionToken::Ident, StringRef(s.data() + pos - len, len) };
}

VersionToken
VersionLexer::consumeNum() {
  usize len = 0;
  u64 value = 0;
  while (pos < s.size() && std::isdigit(s[pos])) {
    if (len > 0 && value == 0) {
      throw SemverError(
          s, '\n', String(pos - len, ' '), "^ invalid leading zero"
      );
    }

    const u64 digit = s[pos] - '0';
    // Check for overflow
    if (value > (std::numeric_limits<u64>::max() - digit) / 10) {
      throw SemverError(
          s, '\n', String(pos - len, ' '), String(len, '^'),
          " number exceeds UINT64_MAX"
      );
    }

    value = value * 10 + digit;
    step();
    ++len;
  }
  return { VersionToken::Num, value };
}

// Note that 012 is an invalid number but 012d is a valid identifier.
VersionToken
VersionLexer::consumeNumOrIdent() {
  const usize oldPos = pos; // we need two passes
  bool isIdent = false;
  while (pos < s.size() && (std::isalnum(s[pos]) || s[pos] == '-')) {
    if (!std::isdigit(s[pos])) {
      isIdent = true;
    }
    step();
  }

  pos = oldPos;
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

  const char c = s[pos];
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
  const usize oldPos = pos;
  const VersionToken tok = next();
  pos = oldPos;
  return tok;
}

struct SemverParseError : public SemverError {
  SemverParseError(
      const VersionLexer& lexer, const VersionToken& tok, const StringRef msg
  )
      : SemverError(lexer.s, '\n', String(lexer.pos, ' '), carets(tok), msg) {}
};

Version
VersionParser::parse() {
  if (lexer.peek().kind == VersionToken::Eof) {
    throw SemverError("empty string is not a valid semver");
  }

  Version v;
  v.major = parseNum();
  parseDot();
  v.minor = parseNum();
  parseDot();
  v.patch = parseNum();

  if (lexer.peek().kind == VersionToken::Hyphen) {
    lexer.step();
    v.pre = parsePre();
  } else {
    v.pre = Prerelease();
  }

  if (lexer.peek().kind == VersionToken::Plus) {
    lexer.step();
    v.build = parseBuild();
  } else {
    v.build = BuildMetadata();
  }

  if (!lexer.isEof()) {
    throw SemverParseError(
        lexer, lexer.peek(),
        " unexpected character: `" + String(1, lexer.s[lexer.pos]) + '`'
    );
  }

  return v;
}

// Even if the token can be parsed as an identifier, try to parse it as a
// number.
u64
VersionParser::parseNum() {
  if (!std::isdigit(lexer.s[lexer.pos])) {
    throw SemverParseError(lexer, lexer.peek(), " expected number");
  }
  return std::get<u64>(lexer.consumeNum().value);
}

void
VersionParser::parseDot() {
  const VersionToken tok = lexer.next();
  if (tok.kind != VersionToken::Dot) {
    throw SemverParseError(lexer, tok, " expected `.`");
  }
}

// pre ::= numOrIdent ("." numOrIdent)*
Prerelease
VersionParser::parsePre() {
  Vec<VersionToken> pre;
  pre.emplace_back(parseNumOrIdent());
  while (lexer.peek().kind == VersionToken::Dot) {
    lexer.step();
    pre.emplace_back(parseNumOrIdent());
  }
  return Prerelease{ pre };
}

// numOrIdent ::= num | ident
VersionToken
VersionParser::parseNumOrIdent() {
  const VersionToken tok = lexer.next();
  if (tok.kind != VersionToken::Num && tok.kind != VersionToken::Ident) {
    throw SemverParseError(lexer, tok, " expected number or identifier");
  }
  return tok;
}

// build ::= ident ("." ident)*
BuildMetadata
VersionParser::parseBuild() {
  Vec<VersionToken> build;
  build.emplace_back(parseIdent());
  while (lexer.peek().kind == VersionToken::Dot) {
    lexer.step();
    build.emplace_back(parseIdent());
  }
  return BuildMetadata{ build };
}

// Even if the token can be parsed as a number, try to parse it as an
// identifier.
VersionToken
VersionParser::parseIdent() {
  if (!std::isalnum(lexer.s[lexer.pos])) {
    throw SemverParseError(lexer, lexer.peek(), " expected identifier");
  }
  return lexer.consumeIdent();
}

Prerelease
Prerelease::parse(const StringRef s) {
  VersionParser parser(s);
  return parser.parsePre();
}

BuildMetadata
BuildMetadata::parse(const StringRef s) {
  VersionParser parser(s);
  return parser.parseBuild();
}

Version
Version::parse(const StringRef s) {
  VersionParser parser(s);
  return parser.parse();
}

#ifdef POAC_TEST

#  include "TestUtils.hpp"

namespace tests {

// Thanks to:
// https://github.com/dtolnay/semver/blob/55fa2cadd6ec95be02e5a2a87b24355304e44d40/tests/test_version.rs#L13

void
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
      Version::parse("1.2.3"),
      (Version{ 1, 2, 3, Prerelease(), BuildMetadata() })
  );
  assertEq(
      Version::parse("1.2.3-alpha1"),
      (Version{ 1, 2, 3, Prerelease::parse("alpha1"), BuildMetadata() })
  );
  assertEq(
      Version::parse("1.2.3+build5"),
      (Version{ 1, 2, 3, Prerelease(), BuildMetadata::parse("build5") })
  );
  assertEq(
      Version::parse("1.2.3+5build"),
      (Version{ 1, 2, 3, Prerelease(), BuildMetadata::parse("5build") })
  );
  assertEq(
      Version::parse("1.2.3-alpha1+build5"),
      (Version{ 1, 2, 3, Prerelease::parse("alpha1"),
                BuildMetadata::parse("build5") })
  );
  assertEq(
      Version::parse("1.2.3-1.alpha1.9+build5.7.3aedf"),
      (Version{ 1, 2, 3, Prerelease::parse("1.alpha1.9"),
                BuildMetadata::parse("build5.7.3aedf") })
  );
  assertEq(
      Version::parse("1.2.3-0a.alpha1.9+05build.7.3aedf"),
      (Version{ 1, 2, 3, Prerelease::parse("0a.alpha1.9"),
                BuildMetadata::parse("05build.7.3aedf") })
  );
  assertEq(
      Version::parse("0.4.0-beta.1+0851523"),
      (Version{ 0, 4, 0, Prerelease::parse("beta.1"),
                BuildMetadata::parse("0851523") })
  );
  assertEq(
      Version::parse("1.1.0-beta-10"),
      (Version{ 1, 1, 0, Prerelease::parse("beta-10"), BuildMetadata() })
  );

  pass();
}

void
testEq() {
  assertEq(Version::parse("1.2.3"), Version::parse("1.2.3"));
  assertEq(Version::parse("1.2.3-alpha1"), Version::parse("1.2.3-alpha1"));
  assertEq(Version::parse("1.2.3+build.42"), Version::parse("1.2.3+build.42"));
  assertEq(
      Version::parse("1.2.3-alpha1+42"), Version::parse("1.2.3-alpha1+42")
  );

  pass();
}

void
testNe() {
  assertNe(Version::parse("0.0.0"), Version::parse("0.0.1"));
  assertNe(Version::parse("0.0.0"), Version::parse("0.1.0"));
  assertNe(Version::parse("0.0.0"), Version::parse("1.0.0"));
  assertNe(Version::parse("1.2.3-alpha"), Version::parse("1.2.3-beta"));
  assertNe(Version::parse("1.2.3+23"), Version::parse("1.2.3+42"));

  pass();
}

void
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

void
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

void
testLe() {
  assertTrue(Version::parse("0.0.0") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.0.0") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.2.0") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.2.3-alpha1") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.2.3-alpha2") <= Version::parse("1.2.3-alpha2"));
  assertTrue(Version::parse("1.2.3+23") <= Version::parse("1.2.3+42"));

  pass();
}

void
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

void
testGe() {
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("0.0.0"));
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("1.0.0"));
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("1.2.0"));
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("1.2.3-alpha1"));
  assertTrue(Version::parse("1.2.3-alpha2") >= Version::parse("1.2.3-alpha2"));
  assertFalse(Version::parse("1.2.3+23") >= Version::parse("1.2.3+42"));

  pass();
}

void
testSpecOrder() {
  const Vec<String> vs = {
    "1.0.0-alpha",  "1.0.0-alpha.1", "1.0.0-alpha.beta", "1.0.0-beta",
    "1.0.0-beta.2", "1.0.0-beta.11", "1.0.0-rc.1",       "1.0.0",
  };
  for (usize i = 1; i < vs.size(); ++i) {
    assertLt(Version::parse(vs[i - 1]), Version::parse(vs[i]));
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
