#include "Semver.hpp"

#include "Rustify.hpp"

#include <cctype>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

std::ostream& operator<<(std::ostream& os, const Token& tok) {
  switch (tok.kind) {
    case Token::Num:
      os << std::get<u64>(tok.value);
      break;
    case Token::Ident:
      os << std::get<StringRef>(tok.value);
      break;
    case Token::Dot:
      os << '.';
      break;
    case Token::Hyphen:
      os << '-';
      break;
    case Token::Plus:
      os << '+';
      break;
    case Token::Eof:
      break;
  }
  return os;
}

String Token::to_string() const {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

usize Token::size() const {
  return to_string().size();
}

bool operator==(const Token& lhs, const Token& rhs) {
  if (lhs.kind != rhs.kind) {
    return false;
  }
  switch (lhs.kind) {
    case Token::Num:
      return std::get<u64>(lhs.value) == std::get<u64>(rhs.value);
    case Token::Ident:
      return std::get<StringRef>(lhs.value) == std::get<StringRef>(rhs.value);
    case Token::Dot:
    case Token::Hyphen:
    case Token::Plus:
    case Token::Eof:
      return true;
  }
  return false;
}

bool operator<(const Token& lhs, const Token& rhs) {
  if (lhs.kind == Token::Num && rhs.kind == Token::Num) {
    return std::get<u64>(lhs.value) < std::get<u64>(rhs.value);
  }
  return lhs.to_string() < rhs.to_string();
}
bool operator>(const Token& lhs, const Token& rhs) {
  return rhs < lhs;
}

String carets(const Token& tok) {
  if (tok.kind != Token::Eof) {
    return String(tok.size(), '^');
  } else {
    return "^";
  }
}

bool Prerelease::empty() const {
  return ident.empty();
}

String Prerelease::to_string() const {
  String s;
  for (usize i = 0; i < ident.size(); ++i) {
    if (i > 0) {
      s += '.';
    }
    s += ident[i].to_string();
  }
  return s;
}

bool operator==(const Prerelease& lhs, const Prerelease& rhs) {
  return lhs.ident == rhs.ident;
}
bool operator<(const Prerelease& lhs, const Prerelease& rhs) {
  for (usize i = 0; i < lhs.ident.size() && i < rhs.ident.size(); ++i) {
    if (lhs.ident[i] < rhs.ident[i]) {
      return true;
    } else if (lhs.ident[i] > rhs.ident[i]) {
      return false;
    }
  }
  return lhs.ident.size() < rhs.ident.size();
}
bool operator>(const Prerelease& lhs, const Prerelease& rhs) {
  return rhs < lhs;
}

bool BuildMetadata::empty() const {
  return ident.empty();
}

String BuildMetadata::to_string() const {
  String s;
  for (usize i = 0; i < ident.size(); ++i) {
    if (i > 0) {
      s += '.';
    }
    s += ident[i].to_string();
  }
  return s;
}

bool operator==(const BuildMetadata& lhs, const BuildMetadata& rhs) {
  return lhs.ident == rhs.ident;
}
bool operator<(const BuildMetadata& lhs, const BuildMetadata& rhs) {
  for (usize i = 0; i < lhs.ident.size() && i < rhs.ident.size(); ++i) {
    if (lhs.ident[i] < rhs.ident[i]) {
      return true;
    } else if (lhs.ident[i] > rhs.ident[i]) {
      return false;
    }
  }
  return lhs.ident.size() < rhs.ident.size();
}
bool operator>(const BuildMetadata& lhs, const BuildMetadata& rhs) {
  return rhs < lhs;
}

String Version::to_string() const {
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

std::ostream& operator<<(std::ostream& os, const Version& v) {
  os << v.to_string();
  return os;
}

bool operator==(const Version& lhs, const Version& rhs) {
  return lhs.major == rhs.major && lhs.minor == rhs.minor
         && lhs.patch == rhs.patch && lhs.pre == rhs.pre
         && lhs.build == rhs.build;
}
bool operator!=(const Version& lhs, const Version& rhs) {
  return !(lhs == rhs);
}

bool operator<(const Version& lhs, const Version& rhs) {
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
bool operator>(const Version& lhs, const Version& rhs) {
  return rhs < lhs;
}
bool operator<=(const Version& lhs, const Version& rhs) {
  return !(rhs < lhs);
}
bool operator>=(const Version& lhs, const Version& rhs) {
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

struct Lexer {
  StringRef s;
  usize pos;

  explicit Lexer(StringRef s) : s(s), pos(0) {}

  bool isEof() const {
    return pos >= s.size();
  }

  void step() {
    if (isEof()) {
      throw SemverException(s, '\n', String(pos, ' '), "^ unexpected eof");
    }
    ++pos;
  }

  Token consumeIdent() {
    usize len = 0;
    while (pos < s.size() && (std::isalnum(s[pos]) || s[pos] == '-')) {
      ++pos;
      ++len;
    }
    return {Token::Ident, StringRef(s.data() + pos - len, len)};
  }

  Token consumeNum() {
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
    return {Token::Num, value};
  }

  // Note that 012 is an invalid number but 012d is a valid identifier.
  Token consumeNumOrIdent() {
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

  Token next() {
    if (isEof()) {
      return Token{Token::Eof};
    }

    const char c = s[pos];
    if (std::isalpha(c)) {
      return consumeIdent();
    } else if (std::isdigit(c)) {
      return consumeNumOrIdent();
    } else if (c == '.') {
      step();
      return Token{Token::Dot};
    } else if (c == '-') {
      step();
      return Token{Token::Hyphen};
    } else if (c == '+') {
      step();
      return Token{Token::Plus};
    } else {
      throw SemverException(
          s, '\n', String(pos, ' '), "^ unexpected character: `", c, '`'
      );
    }
  }

  Token peek() {
    const usize oldPos = pos;
    const Token tok = next();
    pos = oldPos;
    return tok;
  }
};

struct SemverParseException : public SemverException {
  SemverParseException(const Lexer& lexer, const Token& tok, StringRef msg)
      : SemverException(
          lexer.s, '\n', String(lexer.pos, ' '), carets(tok), msg
      ) {}
};

struct Parser {
  Lexer lexer;

  explicit Parser(StringRef s) : lexer(s) {}

  Version parse() {
    if (lexer.peek().kind == Token::Eof) {
      throw SemverException("empty string is not a valid semver");
    }

    Version v;
    v.major = parseNum();
    parseDot();
    v.minor = parseNum();
    parseDot();
    v.patch = parseNum();

    if (lexer.peek().kind == Token::Hyphen) {
      lexer.step();
      v.pre = parsePre();
    } else {
      v.pre = Prerelease();
    }

    if (lexer.peek().kind == Token::Plus) {
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
  u64 parseNum() {
    if (!std::isdigit(lexer.s[lexer.pos])) {
      throw SemverParseException(lexer, lexer.peek(), " expected number");
    }
    return std::get<u64>(lexer.consumeNum().value);
  }

  void parseDot() {
    const Token tok = lexer.next();
    if (tok.kind != Token::Dot) {
      throw SemverParseException(lexer, tok, " expected `.`");
    }
  }

  // pre ::= numOrIdent ("." numOrIdent)*
  Prerelease parsePre() {
    Vec<Token> pre;
    pre.emplace_back(parseNumOrIdent());
    while (lexer.peek().kind == Token::Dot) {
      lexer.step();
      pre.emplace_back(parseNumOrIdent());
    }
    return Prerelease{pre};
  }

  // numOrIdent ::= num | ident
  Token parseNumOrIdent() {
    const Token tok = lexer.next();
    if (tok.kind != Token::Num && tok.kind != Token::Ident) {
      throw SemverParseException(lexer, tok, " expected number or identifier");
    }
    return tok;
  }

  // build ::= ident ("." ident)*
  BuildMetadata parseBuild() {
    Vec<Token> build;
    build.emplace_back(parseIdent());
    while (lexer.peek().kind == Token::Dot) {
      lexer.step();
      build.emplace_back(parseIdent());
    }
    return BuildMetadata{build};
  }

  // Even if the token can be parsed as a number, try to parse it as an
  // identifier.
  Token parseIdent() {
    if (!std::isalnum(lexer.s[lexer.pos])) {
      throw SemverParseException(lexer, lexer.peek(), " expected identifier");
    }
    return lexer.consumeIdent();
  }
};

Prerelease Prerelease::parse(StringRef s) {
  Parser parser(s);
  return parser.parsePre();
}

BuildMetadata BuildMetadata::parse(StringRef s) {
  Parser parser(s);
  return parser.parseBuild();
}

Version parseSemver(StringRef s) {
  Parser parser(s);
  return parser.parse();
}

#ifdef POAC_TEST

#  include "TestUtils.hpp"

// Thanks to:
// https://github.com/dtolnay/semver/blob/55fa2cadd6ec95be02e5a2a87b24355304e44d40/tests/test_version.rs#L13

DEFINE_TEST(test_parse) {
  ASSERT_EXCEPTION(
      parseSemver(""), SemverException,
      "invalid semver:\n"
      "empty string is not a valid semver"
  );
  ASSERT_EXCEPTION(
      parseSemver("  "), SemverException,
      "invalid semver:\n"
      "  \n"
      "^ unexpected character: ` `"
  );
  ASSERT_EXCEPTION(
      parseSemver("1"), SemverException,
      "invalid semver:\n"
      "1\n"
      " ^ expected `.`"
  );
  ASSERT_EXCEPTION(
      parseSemver("1.2"), SemverException,
      "invalid semver:\n"
      "1.2\n"
      "   ^ expected `.`"
  );
  ASSERT_EXCEPTION(
      parseSemver("1.2.3-"), SemverException,
      "invalid semver:\n"
      "1.2.3-\n"
      "      ^ expected number or identifier"
  );
  ASSERT_EXCEPTION(
      parseSemver("a.b.c"), SemverException,
      "invalid semver:\n"
      "a.b.c\n"
      "^ expected number"
  );
  ASSERT_EXCEPTION(
      parseSemver("1.2.3 abc"), SemverException,
      "invalid semver:\n"
      "1.2.3 abc\n"
      "     ^ unexpected character: ` `"
  );
  ASSERT_EXCEPTION(
      parseSemver("1.2.3-01"), SemverException,
      "invalid semver:\n"
      "1.2.3-01\n"
      "      ^ invalid leading zero"
  );
  ASSERT_EXCEPTION(
      parseSemver("1.2.3++"), SemverException,
      "invalid semver:\n"
      "1.2.3++\n"
      "      ^ expected identifier"
  );
  ASSERT_EXCEPTION(
      parseSemver("07"), SemverException,
      "invalid semver:\n"
      "07\n"
      "^ invalid leading zero"
  );
  ASSERT_EXCEPTION(
      parseSemver("111111111111111111111.0.0"), SemverException,
      "invalid semver:\n"
      "111111111111111111111.0.0\n"
      "^ number exceeds UINT64_MAX"
  );
  ASSERT_EXCEPTION(
      parseSemver("8\0"), SemverException,
      "invalid semver:\n"
      "8\n"
      " ^ expected `.`"
  );

  ASSERT_EQ(
      parseSemver("1.2.3"), (Version{1, 2, 3, Prerelease(), BuildMetadata()})
  );
  ASSERT_EQ(
      parseSemver("1.2.3-alpha1"),
      (Version{1, 2, 3, Prerelease::parse("alpha1"), BuildMetadata()})
  );
  ASSERT_EQ(
      parseSemver("1.2.3+build5"),
      (Version{1, 2, 3, Prerelease(), BuildMetadata::parse("build5")})
  );
  ASSERT_EQ(
      parseSemver("1.2.3+5build"),
      (Version{1, 2, 3, Prerelease(), BuildMetadata::parse("5build")})
  );
  ASSERT_EQ(
      parseSemver("1.2.3-alpha1+build5"),
      (Version{
          1, 2, 3, Prerelease::parse("alpha1"), BuildMetadata::parse("build5")
      })
  );
  ASSERT_EQ(
      parseSemver("1.2.3-1.alpha1.9+build5.7.3aedf"),
      (Version{
          1, 2, 3, Prerelease::parse("1.alpha1.9"),
          BuildMetadata::parse("build5.7.3aedf")
      })
  );
  ASSERT_EQ(
      parseSemver("1.2.3-0a.alpha1.9+05build.7.3aedf"),
      (Version{
          1, 2, 3, Prerelease::parse("0a.alpha1.9"),
          BuildMetadata::parse("05build.7.3aedf")
      })
  );
  ASSERT_EQ(
      parseSemver("0.4.0-beta.1+0851523"),
      (Version{
          0, 4, 0, Prerelease::parse("beta.1"), BuildMetadata::parse("0851523")
      })
  );
  ASSERT_EQ(
      parseSemver("1.1.0-beta-10"),
      (Version{1, 1, 0, Prerelease::parse("beta-10"), BuildMetadata()})
  );
}
END_TEST

DEFINE_TEST(test_eq) {
  ASSERT_EQ(parseSemver("1.2.3"), parseSemver("1.2.3"));
  ASSERT_EQ(parseSemver("1.2.3-alpha1"), parseSemver("1.2.3-alpha1"));
  ASSERT_EQ(parseSemver("1.2.3+build.42"), parseSemver("1.2.3+build.42"));
  ASSERT_EQ(parseSemver("1.2.3-alpha1+42"), parseSemver("1.2.3-alpha1+42"));
}
END_TEST

DEFINE_TEST(test_ne) {
  ASSERT_NE(parseSemver("0.0.0"), parseSemver("0.0.1"));
  ASSERT_NE(parseSemver("0.0.0"), parseSemver("0.1.0"));
  ASSERT_NE(parseSemver("0.0.0"), parseSemver("1.0.0"));
  ASSERT_NE(parseSemver("1.2.3-alpha"), parseSemver("1.2.3-beta"));
  ASSERT_NE(parseSemver("1.2.3+23"), parseSemver("1.2.3+42"));
}
END_TEST

DEFINE_TEST(test_display) {
  {
    std::ostringstream oss;
    oss << parseSemver("1.2.3");
    ASSERT_EQ(oss.str(), "1.2.3");
  }
  {
    std::ostringstream oss;
    oss << parseSemver("1.2.3-alpha1");
    ASSERT_EQ(oss.str(), "1.2.3-alpha1");
  }
  {
    std::ostringstream oss;
    oss << parseSemver("1.2.3+build.42");
    ASSERT_EQ(oss.str(), "1.2.3+build.42");
  }
  {
    std::ostringstream oss;
    oss << parseSemver("1.2.3-alpha1+42");
    ASSERT_EQ(oss.str(), "1.2.3-alpha1+42");
  }
}
END_TEST

DEFINE_TEST(test_lt) {
  ASSERT_LT(parseSemver("0.0.0"), parseSemver("1.2.3-alpha2"));
  ASSERT_LT(parseSemver("1.0.0"), parseSemver("1.2.3-alpha2"));
  ASSERT_LT(parseSemver("1.2.0"), parseSemver("1.2.3-alpha2"));
  ASSERT_LT(parseSemver("1.2.3-alpha1"), parseSemver("1.2.3"));
  ASSERT_LT(parseSemver("1.2.3-alpha1"), parseSemver("1.2.3-alpha2"));
  ASSERT_FALSE(parseSemver("1.2.3-alpha2") < parseSemver("1.2.3-alpha2"));
  ASSERT_LT(parseSemver("1.2.3+23"), parseSemver("1.2.3+42"));
}
END_TEST

DEFINE_TEST(test_le) {
  ASSERT_TRUE(parseSemver("0.0.0") <= parseSemver("1.2.3-alpha2"));
  ASSERT_TRUE(parseSemver("1.0.0") <= parseSemver("1.2.3-alpha2"));
  ASSERT_TRUE(parseSemver("1.2.0") <= parseSemver("1.2.3-alpha2"));
  ASSERT_TRUE(parseSemver("1.2.3-alpha1") <= parseSemver("1.2.3-alpha2"));
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") <= parseSemver("1.2.3-alpha2"));
  ASSERT_TRUE(parseSemver("1.2.3+23") <= parseSemver("1.2.3+42"));
}
END_TEST

DEFINE_TEST(test_gt) {
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") > parseSemver("0.0.0"));
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") > parseSemver("1.0.0"));
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") > parseSemver("1.2.0"));
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") > parseSemver("1.2.3-alpha1"));
  ASSERT_TRUE(parseSemver("1.2.3") > parseSemver("1.2.3-alpha2"));
  ASSERT_FALSE(parseSemver("1.2.3-alpha2") > parseSemver("1.2.3-alpha2"));
  ASSERT_FALSE(parseSemver("1.2.3+23") > parseSemver("1.2.3+42"));
}
END_TEST

DEFINE_TEST(test_ge) {
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") >= parseSemver("0.0.0"));
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") >= parseSemver("1.0.0"));
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") >= parseSemver("1.2.0"));
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") >= parseSemver("1.2.3-alpha1"));
  ASSERT_TRUE(parseSemver("1.2.3-alpha2") >= parseSemver("1.2.3-alpha2"));
  ASSERT_FALSE(parseSemver("1.2.3+23") >= parseSemver("1.2.3+42"));
}
END_TEST

DEFINE_TEST(test_spec_order) {
  const Vec<String> vs = {
      "1.0.0-alpha",  "1.0.0-alpha.1", "1.0.0-alpha.beta", "1.0.0-beta",
      "1.0.0-beta.2", "1.0.0-beta.11", "1.0.0-rc.1",       "1.0.0",
  };
  for (usize i = 1; i < vs.size(); ++i) {
    ASSERT_LT(parseSemver(vs[i - 1]), parseSemver(vs[i]));
  }
}
END_TEST

int main() {
  test_parse();
  test_eq();
  test_ne();
  test_display();
  test_lt();
  test_le();
  test_gt();
  test_ge();
  test_spec_order();
}

#endif
