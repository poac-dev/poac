#include "Semver.hpp"

#include "Rustify.hpp"

#include <cctype>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

std::ostream& operator<<(std::ostream& os, const Version& v) {
  os << v.major;
  if (v.minor) {
    os << '.' << v.minor.value();
  }
  if (v.patch) {
    os << '.' << v.patch.value();
  }
  if (v.pre) {
    os << '-' << v.pre.value();
  }
  if (v.build) {
    os << '+' << v.build.value();
  }
  return os;
}

bool operator==(const Version& lhs, const Version& rhs) {
  return lhs.major == rhs.major && lhs.minor == rhs.minor
         && lhs.patch == rhs.patch && lhs.pre == rhs.pre
         && lhs.build == rhs.build;
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

struct Token {
  enum Kind {
    Num, // [1-9][0-9]*
    Ident, // [a-zA-Z0-9]+
    Dot, // .
    Dash, // -
    Plus, // +
    Eof,
  };

  Kind kind;
  std::variant<std::monostate, u64, StringRef> value;

  Token(Kind kind, std::variant<std::monostate, u64, StringRef> value)
      : kind(kind), value(std::move(value)) {}
  explicit Token(Kind kind) : kind(kind), value(std::monostate{}) {}

  usize size() const {
    switch (kind) {
      case Token::Num:
        return std::to_string(std::get<u64>(value)).size();
      case Token::Ident:
        return std::get<StringRef>(value).size();
      case Token::Dot:
      case Token::Dash:
      case Token::Plus:
        return 1;
      case Token::Eof:
        return 0;
    }
    return 0;
  }
};

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
    case Token::Dash:
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

String toStr(const Token& tok) {
  std::ostringstream oss;
  oss << tok;
  return oss.str();
}

String carets(const Token& tok) {
  if (tok.kind != Token::Eof) {
    return String(tok.size(), '^');
  } else {
    return "^";
  }
}

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

  Token consumeIdent() {
    usize len = 0;
    while (pos < s.size() && std::isalnum(s[pos])) {
      ++pos;
      ++len;
    }
    return {Token::Ident, StringRef(s.data() + pos - len, len)};
  }

  Token next() {
    if (isEof()) {
      return Token{Token::Eof};
    }

    const char c = s[pos];
    if (std::isdigit(c)) {
      return consumeNum();
    } else if (std::isalpha(c)) {
      return consumeIdent();
    } else if (c == '.') {
      step();
      return Token{Token::Dot};
    } else if (c == '-') {
      step();
      return Token{Token::Dash};
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
          lexer.s, '\n', String(lexer.pos - tok.size(), ' '), carets(tok), msg
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
    expectDot();
    v.minor = parseNum();
    expectDot();
    v.patch = parseNum();
    v.pre = parsePre();
    v.build = parseBuild();
    return v;
  }

  u64 parseNum() {
    const Token tok = lexer.next();
    if (tok.kind != Token::Num) {
      throw SemverParseException(lexer, tok, " expected number");
    }
    return std::get<u64>(tok.value);
  }

  void expectDot() {
    const Token tok = lexer.next();
    if (tok.kind != Token::Dot) {
      throw SemverParseException(lexer, tok, " expected `.`");
    }
  }

  // pre ::= "-" (num | ident) block
  Option<String> parsePre() {
    if (lexer.peek().kind != Token::Dash) {
      return None;
    }
    lexer.step();

    String pre = parseIdent();
    // Check for leading zero if this is num
    bool isNum = true;
    for (const char c : pre) {
      if (!std::isdigit(c)) {
        isNum = false;
        break;
      }
    }
    if (isNum && pre.size() > 1 && pre[0] == '0') {
      throw SemverParseException(lexer, lexer.peek(), " invalid leading zero");
    }

    pre += parseBlock();
    return pre;
  }

  // build ::= "+" ident block
  Option<String> parseBuild() {
    if (lexer.peek().kind != Token::Plus) {
      return None;
    }
    lexer.step();
    String build = parseIdent();
    build += parseBlock();
    return build;
  }

  // block ::= (("." | "-") ident)*
  String parseBlock() {
    String block;
    while (true) {
      const Token tok = lexer.peek();
      if (tok.kind == Token::Dot) {
        lexer.step();
        block += '.';
        block += parseIdent();
      } else if (tok.kind == Token::Dash) {
        lexer.step();
        block += '-';
        block += parseIdent();
      } else {
        break;
      }
    }
    return block;
  }

  String parseIdent() {
    if (!std::isalnum(lexer.s[lexer.pos])) {
      throw SemverParseException(lexer, lexer.peek(), " expected identifier");
    }
    return toStr(lexer.consumeIdent());
  }
};

Version parseSemver(StringRef s) {
  Parser parser(s);
  return parser.parse();
}

#ifdef POAC_TEST

#  include "TestUtils.hpp"

// Thanks to:
// https://github.com/dtolnay/semver/blob/55fa2cadd6ec95be02e5a2a87b24355304e44d40/tests/test_version.rs#L13

void test_parse() {
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
      "      ^ expected identifier"
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
      "        ^ invalid leading zero"
  );
  ASSERT_EXCEPTION(
      parseSemver("1.2.3++"), SemverException,
      "invalid semver:\n"
      "1.2.3++\n"
      "     ^ expected identifier"
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

  ASSERT_EQ(parseSemver("1.2.3"), (Version{1, 2, 3}));
  ASSERT_EQ(parseSemver("1.2.3-alpha1"), (Version{1, 2, 3, "alpha1"}));
  ASSERT_EQ(parseSemver("1.2.3+build5"), (Version{1, 2, 3, None, "build5"}));
  ASSERT_EQ(parseSemver("1.2.3+5build"), (Version{1, 2, 3, None, "5build"}));
  ASSERT_EQ(
      parseSemver("1.2.3-alpha1+build5"), (Version{1, 2, 3, "alpha1", "build5"})
  );
  ASSERT_EQ(
      parseSemver("1.2.3-1.alpha1.9+build5.7.3aedf"),
      (Version{1, 2, 3, "1.alpha1.9", "build5.7.3aedf"})
  );
  ASSERT_EQ(
      parseSemver("1.2.3-0a.alpha1.9+05build.7.3aedf"),
      (Version{1, 2, 3, "0a.alpha1.9", "05build.7.3aedf"})
  );
  ASSERT_EQ(
      parseSemver("0.4.0-beta.1+0851523"),
      (Version{0, 4, 0, "beta.1", "0851523"})
  );
  ASSERT_EQ(parseSemver("1.1.0-beta-10"), (Version{1, 1, 0, "beta-10"}));
}

void test_eq() {
  ASSERT_EQ(parseSemver("1.2.3"), parseSemver("1.2.3"));
  ASSERT_EQ(parseSemver("1.2.3-alpha1"), parseSemver("1.2.3-alpha1"));
  ASSERT_EQ(parseSemver("1.2.3+build.42"), parseSemver("1.2.3+build.42"));
  ASSERT_EQ(parseSemver("1.2.3-alpha1+42"), parseSemver("1.2.3-alpha1+42"));
}

void test_ne() {
  ASSERT_NE(parseSemver("0.0.0"), parseSemver("0.0.1"));
  ASSERT_NE(parseSemver("0.0.0"), parseSemver("0.1.0"));
  ASSERT_NE(parseSemver("0.0.0"), parseSemver("1.0.0"));
  ASSERT_NE(parseSemver("1.2.3-alpha"), parseSemver("1.2.3-beta"));
  ASSERT_NE(parseSemver("1.2.3+23"), parseSemver("1.2.3+42"));
}

void test_display() {
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

int main() {
  test_parse();
  test_eq();
  test_ne();
  test_display();
}

#endif
