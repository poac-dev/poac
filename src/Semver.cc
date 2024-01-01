#include "Semver.hpp"

#include "Rustify.hpp"

#include <cctype>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

std::ostream& operator<<(std::ostream& os, const VersionToken& tok) noexcept {
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
      break;
  }
  return os;
}

String VersionToken::to_string() const noexcept {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

usize VersionToken::size() const noexcept {
  return to_string().size();
}

bool operator==(const VersionToken& lhs, const VersionToken& rhs) noexcept {
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
      return true;
  }
  return false;
}

bool operator<(const VersionToken& lhs, const VersionToken& rhs) noexcept {
  if (lhs.kind == VersionToken::Num && rhs.kind == VersionToken::Num) {
    return std::get<u64>(lhs.value) < std::get<u64>(rhs.value);
  }
  return lhs.to_string() < rhs.to_string();
}
bool operator>(const VersionToken& lhs, const VersionToken& rhs) {
  return rhs < lhs;
}

static String carets(const VersionToken& tok) noexcept {
  if (tok.kind != VersionToken::Eof) {
    return String(tok.size(), '^');
  } else {
    return "^";
  }
}

bool Prerelease::empty() const noexcept {
  return ident.empty();
}

String Prerelease::to_string() const noexcept {
  String s;
  for (usize i = 0; i < ident.size(); ++i) {
    if (i > 0) {
      s += '.';
    }
    s += ident[i].to_string();
  }
  return s;
}

bool operator==(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  return lhs.ident == rhs.ident;
}
bool operator<(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  for (usize i = 0; i < lhs.ident.size() && i < rhs.ident.size(); ++i) {
    if (lhs.ident[i] < rhs.ident[i]) {
      return true;
    } else if (lhs.ident[i] > rhs.ident[i]) {
      return false;
    }
  }
  return lhs.ident.size() < rhs.ident.size();
}
bool operator>(const Prerelease& lhs, const Prerelease& rhs) noexcept {
  return rhs < lhs;
}

bool BuildMetadata::empty() const noexcept {
  return ident.empty();
}

String BuildMetadata::to_string() const noexcept {
  String s;
  for (usize i = 0; i < ident.size(); ++i) {
    if (i > 0) {
      s += '.';
    }
    s += ident[i].to_string();
  }
  return s;
}

bool operator==(const BuildMetadata& lhs, const BuildMetadata& rhs) noexcept {
  return lhs.ident == rhs.ident;
}
bool operator<(const BuildMetadata& lhs, const BuildMetadata& rhs) noexcept {
  for (usize i = 0; i < lhs.ident.size() && i < rhs.ident.size(); ++i) {
    if (lhs.ident[i] < rhs.ident[i]) {
      return true;
    } else if (lhs.ident[i] > rhs.ident[i]) {
      return false;
    }
  }
  return lhs.ident.size() < rhs.ident.size();
}
bool operator>(const BuildMetadata& lhs, const BuildMetadata& rhs) noexcept {
  return rhs < lhs;
}

String Version::to_string() const noexcept {
  String s = std::to_string(major);
  s += '.' + std::to_string(minor);
  s += '.' + std::to_string(patch);
  if (!pre.empty()) {
    s += '-';
    s += pre.to_string();
  }
  if (!build.empty()) {
    s += '+';
    s += build.to_string();
  }
  return s;
}

std::ostream& operator<<(std::ostream& os, const Version& v) noexcept {
  os << v.to_string();
  return os;
}

bool operator==(const Version& lhs, const Version& rhs) noexcept {
  return lhs.major == rhs.major && lhs.minor == rhs.minor
         && lhs.patch == rhs.patch && lhs.pre == rhs.pre
         && lhs.build == rhs.build;
}
bool operator!=(const Version& lhs, const Version& rhs) noexcept {
  return !(lhs == rhs);
}

bool operator<(const Version& lhs, const Version& rhs) noexcept {
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
bool operator>(const Version& lhs, const Version& rhs) noexcept {
  return rhs < lhs;
}
bool operator<=(const Version& lhs, const Version& rhs) noexcept {
  return !(rhs < lhs);
}
bool operator>=(const Version& lhs, const Version& rhs) noexcept {
  return !(lhs < rhs);
}

struct SemverException : public std::exception {
  template <typename... Args>
  explicit SemverException(Args&&... args) {
    std::ostringstream oss;
    oss << "invalid semver:\n";
    (oss << ... << std::forward<Args>(args));
    what_ = oss.str();
  }

  ~SemverException() noexcept override = default;
  [[nodiscard]] inline const char* what() const noexcept override {
    return what_.c_str();
  }

  SemverException(const SemverException&) = default;
  SemverException& operator=(const SemverException&) = default;
  SemverException(SemverException&&) noexcept = default;
  SemverException& operator=(SemverException&&) noexcept = default;

private:
  String what_;
};

bool VersionLexer::isEof() const noexcept {
  return pos >= s.size();
}

void VersionLexer::step() {
  if (isEof()) {
    throw SemverException(s, '\n', String(pos, ' '), "^ unexpected eof");
  }
  ++pos;
}

VersionToken VersionLexer::consumeIdent() {
  usize len = 0;
  while (pos < s.size() && (std::isalnum(s[pos]) || s[pos] == '-')) {
    ++pos;
    ++len;
  }
  return {VersionToken::Ident, StringRef(s.data() + pos - len, len)};
}

VersionToken VersionLexer::consumeNum() {
  usize len = 0;
  u64 value = 0;
  while (pos < s.size() && std::isdigit(s[pos])) {
    if (len > 0 && value == 0) {
      throw SemverException(
          s, '\n', String(pos - len, ' '), "^ invalid leading zero"
      );
    }

    u64 digit = s[pos] - '0';
    // Check for overflow
    if (value > (std::numeric_limits<u64>::max() - digit) / 10) {
      throw SemverException(
          s, '\n', String(pos - len, ' '), "^ number exceeds UINT64_MAX"
      );
    }

    value = value * 10 + digit;
    ++pos;
    ++len;
  }
  return {VersionToken::Num, value};
}

// Note that 012 is an invalid number but 012d is a valid identifier.
VersionToken VersionLexer::consumeNumOrIdent() {
  const usize oldPos = pos; // we need two passes
  bool isIdent = false;
  while (pos < s.size() && (std::isalnum(s[pos]) || s[pos] == '-')) {
    if (!std::isdigit(s[pos])) {
      isIdent = true;
    }
    ++pos;
  }

  pos = oldPos;
  if (isIdent) {
    return consumeIdent();
  } else {
    return consumeNum();
  }
}

VersionToken VersionLexer::next() {
  if (isEof()) {
    return VersionToken{VersionToken::Eof};
  }

  const char c = s[pos];
  if (std::isalpha(c)) {
    return consumeIdent();
  } else if (std::isdigit(c)) {
    return consumeNumOrIdent();
  } else if (c == '.') {
    step();
    return VersionToken{VersionToken::Dot};
  } else if (c == '-') {
    step();
    return VersionToken{VersionToken::Hyphen};
  } else if (c == '+') {
    step();
    return VersionToken{VersionToken::Plus};
  } else {
    throw SemverException(
        s, '\n', String(pos, ' '), "^ unexpected character: `", c, '`'
    );
  }
}

VersionToken VersionLexer::peek() {
  const usize oldPos = pos;
  const VersionToken tok = next();
  pos = oldPos;
  return tok;
}

struct SemverParseException : public SemverException {
  SemverParseException(
      const VersionLexer& lexer, const VersionToken& tok, StringRef msg
  )
      : SemverException(
          lexer.s, '\n', String(lexer.pos, ' '), carets(tok), msg
      ) {}
};

Version VersionParser::parse() {
  if (lexer.peek().kind == VersionToken::Eof) {
    throw SemverException("empty string is not a valid semver");
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
    throw SemverParseException(
        lexer, lexer.peek(),
        " unexpected character: `" + String(1, lexer.s[lexer.pos]) + '`'
    );
  }

  return v;
}

// Even if the token can be parsed as an identifier, try to parse it as a
// number.
u64 VersionParser::parseNum() {
  if (!std::isdigit(lexer.s[lexer.pos])) {
    throw SemverParseException(lexer, lexer.peek(), " expected number");
  }
  return std::get<u64>(lexer.consumeNum().value);
}

void VersionParser::parseDot() {
  const VersionToken tok = lexer.next();
  if (tok.kind != VersionToken::Dot) {
    throw SemverParseException(lexer, tok, " expected `.`");
  }
}

// pre ::= numOrIdent ("." numOrIdent)*
Prerelease VersionParser::parsePre() {
  Vec<VersionToken> pre;
  pre.emplace_back(parseNumOrIdent());
  while (lexer.peek().kind == VersionToken::Dot) {
    lexer.step();
    pre.emplace_back(parseNumOrIdent());
  }
  return Prerelease{pre};
}

// numOrIdent ::= num | ident
VersionToken VersionParser::parseNumOrIdent() {
  const VersionToken tok = lexer.next();
  if (tok.kind != VersionToken::Num && tok.kind != VersionToken::Ident) {
    throw SemverParseException(lexer, tok, " expected number or identifier");
  }
  return tok;
}

// build ::= ident ("." ident)*
BuildMetadata VersionParser::parseBuild() {
  Vec<VersionToken> build;
  build.emplace_back(parseIdent());
  while (lexer.peek().kind == VersionToken::Dot) {
    lexer.step();
    build.emplace_back(parseIdent());
  }
  return BuildMetadata{build};
}

// Even if the token can be parsed as a number, try to parse it as an
// identifier.
VersionToken VersionParser::parseIdent() {
  if (!std::isalnum(lexer.s[lexer.pos])) {
    throw SemverParseException(lexer, lexer.peek(), " expected identifier");
  }
  return lexer.consumeIdent();
}

Prerelease Prerelease::parse(StringRef s) {
  VersionParser parser(s);
  return parser.parsePre();
}

BuildMetadata BuildMetadata::parse(StringRef s) {
  VersionParser parser(s);
  return parser.parseBuild();
}

Version Version::parse(StringRef s) {
  VersionParser parser(s);
  return parser.parse();
}

#ifdef POAC_TEST

#  include "TestUtils.hpp"

// Thanks to:
// https://github.com/dtolnay/semver/blob/55fa2cadd6ec95be02e5a2a87b24355304e44d40/tests/test_version.rs#L13

void test_parse() {
  ASSERT_EXCEPTION(
      Version::parse(""), SemverException,
      "invalid semver:\n"
      "empty string is not a valid semver"
  );
  ASSERT_EXCEPTION(
      Version::parse("  "), SemverException,
      "invalid semver:\n"
      "  \n"
      "^ unexpected character: ` `"
  );
  ASSERT_EXCEPTION(
      Version::parse("1"), SemverException,
      "invalid semver:\n"
      "1\n"
      " ^ expected `.`"
  );
  ASSERT_EXCEPTION(
      Version::parse("1.2"), SemverException,
      "invalid semver:\n"
      "1.2\n"
      "   ^ expected `.`"
  );
  ASSERT_EXCEPTION(
      Version::parse("1.2.3-"), SemverException,
      "invalid semver:\n"
      "1.2.3-\n"
      "      ^ expected number or identifier"
  );
  ASSERT_EXCEPTION(
      Version::parse("a.b.c"), SemverException,
      "invalid semver:\n"
      "a.b.c\n"
      "^ expected number"
  );
  ASSERT_EXCEPTION(
      Version::parse("1.2.3 abc"), SemverException,
      "invalid semver:\n"
      "1.2.3 abc\n"
      "     ^ unexpected character: ` `"
  );
  ASSERT_EXCEPTION(
      Version::parse("1.2.3-01"), SemverException,
      "invalid semver:\n"
      "1.2.3-01\n"
      "      ^ invalid leading zero"
  );
  ASSERT_EXCEPTION(
      Version::parse("1.2.3++"), SemverException,
      "invalid semver:\n"
      "1.2.3++\n"
      "      ^ expected identifier"
  );
  ASSERT_EXCEPTION(
      Version::parse("07"), SemverException,
      "invalid semver:\n"
      "07\n"
      "^ invalid leading zero"
  );
  ASSERT_EXCEPTION(
      Version::parse("111111111111111111111.0.0"), SemverException,
      "invalid semver:\n"
      "111111111111111111111.0.0\n"
      "^ number exceeds UINT64_MAX"
  );
  ASSERT_EXCEPTION(
      Version::parse("8\0"), SemverException,
      "invalid semver:\n"
      "8\n"
      " ^ expected `.`"
  );

  ASSERT_EQ(
      Version::parse("1.2.3"), (Version{1, 2, 3, Prerelease(), BuildMetadata()})
  );
  ASSERT_EQ(
      Version::parse("1.2.3-alpha1"),
      (Version{1, 2, 3, Prerelease::parse("alpha1"), BuildMetadata()})
  );
  ASSERT_EQ(
      Version::parse("1.2.3+build5"),
      (Version{1, 2, 3, Prerelease(), BuildMetadata::parse("build5")})
  );
  ASSERT_EQ(
      Version::parse("1.2.3+5build"),
      (Version{1, 2, 3, Prerelease(), BuildMetadata::parse("5build")})
  );
  ASSERT_EQ(
      Version::parse("1.2.3-alpha1+build5"),
      (Version{
          1, 2, 3, Prerelease::parse("alpha1"), BuildMetadata::parse("build5")
      })
  );
  ASSERT_EQ(
      Version::parse("1.2.3-1.alpha1.9+build5.7.3aedf"),
      (Version{
          1, 2, 3, Prerelease::parse("1.alpha1.9"),
          BuildMetadata::parse("build5.7.3aedf")
      })
  );
  ASSERT_EQ(
      Version::parse("1.2.3-0a.alpha1.9+05build.7.3aedf"),
      (Version{
          1, 2, 3, Prerelease::parse("0a.alpha1.9"),
          BuildMetadata::parse("05build.7.3aedf")
      })
  );
  ASSERT_EQ(
      Version::parse("0.4.0-beta.1+0851523"),
      (Version{
          0, 4, 0, Prerelease::parse("beta.1"), BuildMetadata::parse("0851523")
      })
  );
  ASSERT_EQ(
      Version::parse("1.1.0-beta-10"),
      (Version{1, 1, 0, Prerelease::parse("beta-10"), BuildMetadata()})
  );
}

void test_eq() {
  ASSERT_EQ(Version::parse("1.2.3"), Version::parse("1.2.3"));
  ASSERT_EQ(Version::parse("1.2.3-alpha1"), Version::parse("1.2.3-alpha1"));
  ASSERT_EQ(Version::parse("1.2.3+build.42"), Version::parse("1.2.3+build.42"));
  ASSERT_EQ(
      Version::parse("1.2.3-alpha1+42"), Version::parse("1.2.3-alpha1+42")
  );
}

void test_ne() {
  ASSERT_NE(Version::parse("0.0.0"), Version::parse("0.0.1"));
  ASSERT_NE(Version::parse("0.0.0"), Version::parse("0.1.0"));
  ASSERT_NE(Version::parse("0.0.0"), Version::parse("1.0.0"));
  ASSERT_NE(Version::parse("1.2.3-alpha"), Version::parse("1.2.3-beta"));
  ASSERT_NE(Version::parse("1.2.3+23"), Version::parse("1.2.3+42"));
}

void test_display() {
  {
    std::ostringstream oss;
    oss << Version::parse("1.2.3");
    ASSERT_EQ(oss.str(), "1.2.3");
  }
  {
    std::ostringstream oss;
    oss << Version::parse("1.2.3-alpha1");
    ASSERT_EQ(oss.str(), "1.2.3-alpha1");
  }
  {
    std::ostringstream oss;
    oss << Version::parse("1.2.3+build.42");
    ASSERT_EQ(oss.str(), "1.2.3+build.42");
  }
  {
    std::ostringstream oss;
    oss << Version::parse("1.2.3-alpha1+42");
    ASSERT_EQ(oss.str(), "1.2.3-alpha1+42");
  }
}

void test_lt() {
  ASSERT_LT(Version::parse("0.0.0"), Version::parse("1.2.3-alpha2"));
  ASSERT_LT(Version::parse("1.0.0"), Version::parse("1.2.3-alpha2"));
  ASSERT_LT(Version::parse("1.2.0"), Version::parse("1.2.3-alpha2"));
  ASSERT_LT(Version::parse("1.2.3-alpha1"), Version::parse("1.2.3"));
  ASSERT_LT(Version::parse("1.2.3-alpha1"), Version::parse("1.2.3-alpha2"));
  ASSERT_FALSE(Version::parse("1.2.3-alpha2") < Version::parse("1.2.3-alpha2"));
  ASSERT_LT(Version::parse("1.2.3+23"), Version::parse("1.2.3+42"));
}

void test_le() {
  ASSERT_TRUE(Version::parse("0.0.0") <= Version::parse("1.2.3-alpha2"));
  ASSERT_TRUE(Version::parse("1.0.0") <= Version::parse("1.2.3-alpha2"));
  ASSERT_TRUE(Version::parse("1.2.0") <= Version::parse("1.2.3-alpha2"));
  ASSERT_TRUE(Version::parse("1.2.3-alpha1") <= Version::parse("1.2.3-alpha2"));
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") <= Version::parse("1.2.3-alpha2"));
  ASSERT_TRUE(Version::parse("1.2.3+23") <= Version::parse("1.2.3+42"));
}

void test_gt() {
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") > Version::parse("0.0.0"));
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") > Version::parse("1.0.0"));
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") > Version::parse("1.2.0"));
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") > Version::parse("1.2.3-alpha1"));
  ASSERT_TRUE(Version::parse("1.2.3") > Version::parse("1.2.3-alpha2"));
  ASSERT_FALSE(Version::parse("1.2.3-alpha2") > Version::parse("1.2.3-alpha2"));
  ASSERT_FALSE(Version::parse("1.2.3+23") > Version::parse("1.2.3+42"));
}

void test_ge() {
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") >= Version::parse("0.0.0"));
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") >= Version::parse("1.0.0"));
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") >= Version::parse("1.2.0"));
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") >= Version::parse("1.2.3-alpha1"));
  ASSERT_TRUE(Version::parse("1.2.3-alpha2") >= Version::parse("1.2.3-alpha2"));
  ASSERT_FALSE(Version::parse("1.2.3+23") >= Version::parse("1.2.3+42"));
}

void test_spec_order() {
  const Vec<String> vs = {
      "1.0.0-alpha",  "1.0.0-alpha.1", "1.0.0-alpha.beta", "1.0.0-beta",
      "1.0.0-beta.2", "1.0.0-beta.11", "1.0.0-rc.1",       "1.0.0",
  };
  for (usize i = 1; i < vs.size(); ++i) {
    ASSERT_LT(Version::parse(vs[i - 1]), Version::parse(vs[i]));
  }
}

int main() {
  REGISTER_TEST(test_parse);
  REGISTER_TEST(test_eq);
  REGISTER_TEST(test_ne);
  REGISTER_TEST(test_display);
  REGISTER_TEST(test_lt);
  REGISTER_TEST(test_le);
  REGISTER_TEST(test_gt);
  REGISTER_TEST(test_ge);
  REGISTER_TEST(test_spec_order);
}

#endif
