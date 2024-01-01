#include "VersionReq.hpp"

#include "Rustify.hpp"

#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <variant>

struct ComparatorException : public std::exception {
  template <typename... Args>
  explicit ComparatorException(Args&&... args) {
    std::ostringstream oss;
    oss << "invalid comparator:\n";
    (oss << ... << std::forward<Args>(args));
    what_ = oss.str();
  }

  ~ComparatorException() noexcept override = default;
  [[nodiscard]] inline const char* what() const noexcept override {
    return what_.c_str();
  }

  ComparatorException(const ComparatorException&) = default;
  ComparatorException& operator=(const ComparatorException&) = default;
  ComparatorException(ComparatorException&&) noexcept = default;
  ComparatorException& operator=(ComparatorException&&) noexcept = default;

private:
  String what_;
};

struct VersionReqException : public std::exception {
  template <typename... Args>
  explicit VersionReqException(Args&&... args) {
    std::ostringstream oss;
    oss << "invalid version requirement:\n";
    (oss << ... << std::forward<Args>(args));
    what_ = oss.str();
  }

  ~VersionReqException() noexcept override = default;
  [[nodiscard]] inline const char* what() const noexcept override {
    return what_.c_str();
  }

  VersionReqException(const VersionReqException&) = default;
  VersionReqException& operator=(const VersionReqException&) = default;
  VersionReqException(VersionReqException&&) noexcept = default;
  VersionReqException& operator=(VersionReqException&&) noexcept = default;

private:
  String what_;
};

String to_string(const Comparator::Op op) noexcept {
  switch (op) {
    case Comparator::Exact:
      return "=";
    case Comparator::Gt:
      return ">";
    case Comparator::Gte:
      return ">=";
    case Comparator::Lt:
      return "<";
    case Comparator::Lte:
      return "<=";
  }
}

struct OptVersion {
  u64 major;
  Option<u64> minor;
  Option<u64> patch;
  Prerelease pre;
};

struct ComparatorToken {
  enum Kind {
    Eq, // =
    Gt, // >
    Gte, // >=
    Lt, // <
    Lte, // <=
    Ver, // OptVersion
    Eof,
    Unknown,
  };

  Kind kind;
  std::variant<std::monostate, OptVersion> value;

  ComparatorToken(
      Kind kind, std::variant<std::monostate, OptVersion> value
  ) noexcept
      : kind(kind), value(std::move(value)) {}

  explicit ComparatorToken(Kind kind) noexcept
      : kind(kind), value(std::monostate{}) {}
};

struct ComparatorLexer {
  StringRef s;
  usize pos;

  explicit ComparatorLexer(StringRef s) noexcept : s(s), pos(0) {}

  bool isEof() const noexcept {
    return pos >= s.size();
  }

  void step() noexcept {
    ++pos;
  }

  ComparatorToken next() {
    if (isEof()) {
      return ComparatorToken{ComparatorToken::Eof};
    }

    const char c = s[pos];
    if (c == '=') {
      step();
      return ComparatorToken{ComparatorToken::Eq};
    } else if (c == '>') {
      step();
      if (isEof()) {
        return ComparatorToken{ComparatorToken::Gt};
      } else if (s[pos] == '=') {
        step();
        return ComparatorToken{ComparatorToken::Gte};
      } else {
        return ComparatorToken{ComparatorToken::Gt};
      }
    } else if (c == '<') {
      step();
      if (isEof()) {
        return ComparatorToken{ComparatorToken::Lt};
      } else if (s[pos] == '=') {
        step();
        return ComparatorToken{ComparatorToken::Lte};
      } else {
        return ComparatorToken{ComparatorToken::Lt};
      }
    } else if (std::isdigit(c)) {
      VersionParser parser(s);
      parser.lexer.pos = pos;

      OptVersion ver;
      ver.major = parser.parseNum();
      if (parser.lexer.s[parser.lexer.pos] != '.') {
        pos = parser.lexer.pos;
        return ComparatorToken{ComparatorToken::Ver, std::move(ver)};
      }

      parser.parseDot();
      ver.minor = parser.parseNum();
      if (parser.lexer.s[parser.lexer.pos] != '.') {
        pos = parser.lexer.pos;
        return ComparatorToken{ComparatorToken::Ver, std::move(ver)};
      }

      parser.parseDot();
      ver.patch = parser.parseNum();

      if (parser.lexer.s[parser.lexer.pos] == '-') {
        parser.lexer.step();
        ver.pre = parser.parsePre();
      }

      if (parser.lexer.s[parser.lexer.pos] == '+') {
        parser.lexer.step();
        parser.parseBuild(); // discard build metadata
      }

      pos = parser.lexer.pos;
      return ComparatorToken{ComparatorToken::Ver, std::move(ver)};
    } else {
      return ComparatorToken{ComparatorToken::Unknown};
    }
  }
};

struct ComparatorParser {
  ComparatorLexer lexer;

  explicit ComparatorParser(StringRef s) : lexer(s) {}

  Comparator parse() {
    Comparator result;

    const auto token = lexer.next();
    if (token.kind == ComparatorToken::Eq) {
      result.op = Comparator::Exact;
    } else if (token.kind == ComparatorToken::Gt) {
      result.op = Comparator::Gt;
    } else if (token.kind == ComparatorToken::Gte) {
      result.op = Comparator::Gte;
    } else if (token.kind == ComparatorToken::Lt) {
      result.op = Comparator::Lt;
    } else if (token.kind == ComparatorToken::Lte) {
      result.op = Comparator::Lte;
    } else if (token.kind == ComparatorToken::Ver) {
      const OptVersion& ver = std::get<OptVersion>(token.value);
      result.major = ver.major;
      result.minor = ver.minor;
      result.patch = ver.patch;
      result.pre = ver.pre;
    } else {
      throw ComparatorException(
          lexer.s, '\n', String(lexer.pos, ' '),
          "^ expected =, >=, <=, >, <, or version"
      );
    }

    // If the first token was comparison operator, the next token must be
    // version.
    if (token.kind != ComparatorToken::Ver) {
      const auto token2 = lexer.next();
      if (token2.kind != ComparatorToken::Ver) {
        throw ComparatorException(
            lexer.s, '\n', String(lexer.pos, ' '), "^ expected version"
        );
      }
      const OptVersion& ver = std::get<OptVersion>(token2.value);
      result.major = ver.major;
      result.minor = ver.minor;
      result.patch = ver.patch;
      result.pre = ver.pre;
    }

    return result;
  }
};

Comparator Comparator::parse(StringRef s) {
  ComparatorParser parser(s);
  return parser.parse();
}

// Dangerously convert to Version.  This method can lose the original
// information.  Do not use unless it makes sense.
Version Comparator::to_version() const noexcept {
  Version ver;
  ver.major = major;
  ver.minor = minor.value_or(0);
  ver.patch = patch.value_or(0);
  ver.pre = pre;
  return ver;
}

String Comparator::to_string() const noexcept {
  String result;
  if (op.has_value()) {
    result += ::to_string(op.value());
  }
  result += std::to_string(major);
  if (minor.has_value()) {
    result += ".";
    result += std::to_string(minor.value());

    if (patch.has_value()) {
      result += ".";
      result += std::to_string(patch.value());

      if (!pre.empty()) {
        result += "-";
        result += pre.to_string();
      }
    }
  }
  return result;
}

static bool matchesExact(const Comparator& cmp, const Version& ver) noexcept {
  if (ver.major != cmp.major) {
    return false;
  }

  if (const auto minor = cmp.minor) {
    if (ver.minor != minor.value()) {
      return false;
    }
  }

  if (const auto patch = cmp.patch) {
    if (ver.patch != patch.value()) {
      return false;
    }
  }

  return ver.pre == cmp.pre;
}

static bool matchesGreater(const Comparator& cmp, const Version& ver) noexcept {
  if (ver.major != cmp.major) {
    return ver.major > cmp.major;
  }

  if (!cmp.minor.has_value()) {
    return false;
  } else {
    const u64 minor = cmp.minor.value();
    if (ver.minor != minor) {
      return ver.minor > minor;
    }
  }

  if (!cmp.patch.has_value()) {
    return false;
  } else {
    const u64 patch = cmp.patch.value();
    if (ver.patch != patch) {
      return ver.patch > patch;
    }
  }

  return ver.pre > cmp.pre;
}

static bool matchesLess(const Comparator& cmp, const Version& ver) noexcept {
  if (ver.major != cmp.major) {
    return ver.major < cmp.major;
  }

  if (!cmp.minor.has_value()) {
    return false;
  } else {
    const u64 minor = cmp.minor.value();
    if (ver.minor != minor) {
      return ver.minor < minor;
    }
  }

  if (!cmp.patch.has_value()) {
    return false;
  } else {
    const u64 patch = cmp.patch.value();
    if (ver.patch != patch) {
      return ver.patch < patch;
    }
  }

  return ver.pre < cmp.pre;
}

static bool matchesNoOp(const Comparator& cmp, const Version& ver) noexcept {
  if (ver.major != cmp.major) {
    return false;
  }

  if (!cmp.minor.has_value()) {
    return true;
  }
  const u64 minor = cmp.minor.value();

  if (!cmp.patch.has_value()) {
    if (cmp.major > 0) {
      return ver.minor >= minor;
    } else {
      return ver.minor == minor;
    }
  }
  const u64 patch = cmp.patch.value();

  if (cmp.major > 0) {
    if (ver.minor != minor) {
      return ver.minor > minor;
    } else if (ver.patch != patch) {
      return ver.patch > patch;
    }
  } else if (minor > 0) {
    if (ver.minor != minor) {
      return false;
    } else if (ver.patch != patch) {
      return ver.patch > patch;
    }
  } else if (ver.minor != minor || ver.patch != patch) {
    return false;
  }

  return ver.pre >= cmp.pre;
}

bool Comparator::satisfiedBy(const Version& ver) const noexcept {
  if (!op.has_value()) { // NoOp
    return matchesNoOp(*this, ver);
  }

  switch (op.value()) {
    case Op::Exact:
      return matchesExact(*this, ver);
    case Op::Gt:
      return matchesGreater(*this, ver);
    case Op::Gte:
      return matchesExact(*this, ver) || matchesGreater(*this, ver);
    case Op::Lt:
      return matchesLess(*this, ver);
    case Op::Lte:
      return matchesExact(*this, ver) || matchesLess(*this, ver);
  }
}

Comparator Comparator::canonicalize() const noexcept {
  if (!op.has_value() || op.value() == Op::Exact) {
    // For NoOp or Exact, canonicalization can be done over VersionReq.
    return *this;
  }

  Comparator cmp = *this;
  const Op op = this->op.value();
  switch (op) {
    case Op::Gt:
      cmp.op = Op::Gte;
      break;
    case Op::Lte:
      cmp.op = Op::Lt;
      break;
    default:
      cmp.minor = cmp.minor.value_or(0);
      cmp.patch = cmp.patch.value_or(0);
      return cmp;
  }

  if (patch.has_value()) {
    cmp.patch = patch.value() + 1;
    return cmp;
  } else {
    cmp.patch = 0;
  }

  if (minor.has_value()) {
    cmp.minor = minor.value() + 1;
    return cmp;
  } else {
    cmp.minor = 0;
  }

  cmp.major += 1;
  return cmp;
}

struct VersionReqToken {
  enum Kind {
    Comp,
    And,
    Eof,
    Unknown,
  };

  Kind kind;
  std::variant<std::monostate, Comparator> value;

  VersionReqToken(
      Kind kind, std::variant<std::monostate, Comparator> value
  ) noexcept
      : kind(kind), value(std::move(value)) {}

  explicit VersionReqToken(Kind kind) noexcept
      : kind(kind), value(std::monostate{}) {}
};

constexpr bool isCompStart(const char c) noexcept {
  return c == '=' || c == '>' || c == '<';
}

struct VersionReqLexer {
  StringRef s;
  usize pos;

  explicit VersionReqLexer(StringRef s) noexcept : s(s), pos(0) {}

  bool isEof() const noexcept {
    return pos >= s.size();
  }

  void skipWs() noexcept {
    while (!isEof() && std::isspace(s[pos])) {
      ++pos;
    }
  }

  VersionReqToken next() {
    skipWs();
    if (isEof()) {
      return VersionReqToken{VersionReqToken::Eof};
    }

    const char c = s[pos];
    if (isCompStart(c) || std::isdigit(c)) {
      ComparatorParser parser(s);
      parser.lexer.pos = pos;

      const Comparator comp = parser.parse();
      pos = parser.lexer.pos;

      return VersionReqToken{VersionReqToken::Comp, comp};
    } else if (c == 'a' && pos + 2 < s.size()) {
      if (s[pos + 1] == 'n' && s[pos + 2] == 'd') {
        pos += 3;
        return VersionReqToken{VersionReqToken::And};
      }
    }

    return VersionReqToken{VersionReqToken::Unknown};
  }
};

struct VersionReqParser {
  VersionReqLexer lexer;

  explicit VersionReqParser(StringRef s) noexcept : lexer(s) {}

  VersionReq parse() {
    VersionReq result;

    result.left = parseComparatorOrOptVer();
    if (!result.left.op.has_value()
        || result.left.op.value() == Comparator::Exact) { // NoOp or Exact
      lexer.skipWs();
      if (!lexer.isEof()) {
        throw VersionReqException(
            lexer.s, '\n', String(lexer.pos, ' '),
            "^ NoOp and Exact cannot chain"
        );
      }
      return result;
    }

    const VersionReqToken token = lexer.next();
    if (token.kind == VersionReqToken::Eof) {
      return result;
    } else if (token.kind != VersionReqToken::And) {
      throw VersionReqException(
          lexer.s, '\n', String(lexer.pos, ' '), "^ expected `and`"
      );
    }

    result.right = parseComparator();
    return result;
  }

  // Parse `("=" | CompOp)? OptVersion` or `Comparator`.
  Comparator parseComparatorOrOptVer() {
    const VersionReqToken token = lexer.next();
    if (token.kind != VersionReqToken::Comp) {
      throw VersionReqException(
          lexer.s, '\n', String(lexer.pos, ' '),
          "^ expected =, >=, <=, >, <, or version"
      );
    }
    return std::get<Comparator>(token.value);
  }

  // If the token is a NoOp or Exact comparator, throw an exception.  This
  // is because NoOp and Exact cannot chain, and the Comparator parser
  // handles both `("=" | CompOp)? OptVersion` and `Comparator` cases for
  // simplicity. That is, this method literally accepts `Comparator` defined
  // in the grammar.  Otherwise, return the comparator if the token is a
  // comparator.
  Comparator parseComparator() {
    lexer.skipWs();
    if (lexer.isEof()) {
      compExpected();
    }
    if (!isCompStart(lexer.s[lexer.pos])) {
      // NoOp cannot chain.
      compExpected();
    }
    if (lexer.s[lexer.pos] == '=') {
      // Exact cannot chain.
      compExpected();
    }

    const VersionReqToken token = lexer.next();
    if (token.kind != VersionReqToken::Comp) {
      compExpected();
    }
    return std::get<Comparator>(token.value);
  }

  [[noreturn]] void compExpected() {
    throw VersionReqException(
        lexer.s, '\n', String(lexer.pos, ' '), "^ expected >=, <=, >, or <"
    );
  }
};

VersionReq VersionReq::parse(StringRef s) {
  VersionReqParser parser(s);
  return parser.parse();
}

bool VersionReq::satisfiedBy(const Version& ver) const noexcept {
  if (!right.has_value()) {
    return left.satisfiedBy(ver);
  }

  return left.satisfiedBy(ver) && right->satisfiedBy(ver);
}

// 1. NoOp: (= Caret (^), "compatible" updates)
//   1.1. `A.B.C` (where A > 0) is equivalent to `>=A.B.C and <(A+1).0.0`
//   1.2. `A.B` (where A > 0 & B > 0) is equivalent to `^A.B.0` (i.e., 1.1)
//   1.3. `A` is equivalent to `=A` (i.e., 2.3)
//   1.4. `0.B.C` (where B > 0) is equivalent to `>=0.B.C and <0.(B+1).0`
//   1.5. `0.0.C` is equivalent to `=0.0.C` (i.e., 2.1)
//   1.6. `0.0` is equivalent to `=0.0` (i.e., 2.2)
static VersionReq canonicalizeNoOp(const VersionReq& target) noexcept {
  const Comparator& left = target.left;

  if (!left.minor.has_value() && !left.patch.has_value()) {
    // {{ !B.has_value() && !C.has_value() }}
    // 1.3. `A` is equivalent to `=A` (i.e., 2.3)
    VersionReq req;
    req.left.op = Comparator::Gte;
    req.left.major = left.major;
    req.left.minor = 0;
    req.left.patch = 0;
    req.left.pre = left.pre;

    req.right = Comparator();
    req.right->op = Comparator::Lt;
    req.right->major = left.major + 1;
    req.right->minor = 0;
    req.right->patch = 0;
    req.right->pre = left.pre;

    return req;
  }
  // => {{ B.has_value() || C.has_value() }}
  // => {{ B.has_value() }} since {{ !B.has_value() && C.has_value() }} is
  //    impossible as the semver parser rejects it.

  if (left.major > 0) { // => {{ A > 0 && B.has_value() }}
    if (left.patch.has_value()) {
      // => {{ A > 0 && B.has_value() && C.has_value() }}
      // 1.1. `A.B.C` (where A > 0) is equivalent to `>=A.B.C and <(A+1).0.0`
      VersionReq req;
      req.left.op = Comparator::Gte;
      req.left.major = left.major;
      req.left.minor = left.minor.value();
      req.left.patch = left.patch.value();
      req.left.pre = left.pre;

      req.right = Comparator();
      req.right->op = Comparator::Lt;
      req.right->major = left.major + 1;
      req.right->minor = 0;
      req.right->patch = 0;
      req.right->pre = left.pre;

      return req;
    } else { // => {{ A > 0 && B.has_value() && !C.has_value() }}
      // 1.2. `A.B` (where A > 0 & B > 0) is equivalent to `^A.B.0` (i.e., 1.1)
      VersionReq req;
      req.left.op = Comparator::Gte;
      req.left.major = left.major;
      req.left.minor = left.minor.value();
      req.left.patch = 0;
      req.left.pre = left.pre;

      req.right = Comparator();
      req.right->op = Comparator::Lt;
      req.right->major = left.major + 1;
      req.right->minor = 0;
      req.right->patch = 0;
      req.right->pre = left.pre;

      return req;
    }
  }
  // => {{ A == 0 && B.has_value() }}

  if (left.minor.value() > 0) { // => {{ A == 0 && B > 0 }}
    // 1.4. `0.B.C` (where B > 0) is equivalent to `>=0.B.C and <0.(B+1).0`
    VersionReq req;
    req.left.op = Comparator::Gte;
    req.left.major = 0;
    req.left.minor = left.minor.value();
    req.left.patch = left.patch.value_or(0);
    req.left.pre = left.pre;

    req.right = Comparator();
    req.right->op = Comparator::Lt;
    req.right->major = 0;
    req.right->minor = left.minor.value() + 1;
    req.right->patch = 0;
    req.right->pre = left.pre;

    return req;
  }
  // => {{ A == 0 && B == 0 }}

  if (left.patch.has_value()) { // => {{ A == 0 && B == 0 && C.has_value() }}
    // 1.5. `0.0.C` is equivalent to `=0.0.C` (i.e., 2.1)
    VersionReq req;
    req.left.op = Comparator::Exact;
    req.left.major = 0;
    req.left.minor = 0;
    req.left.patch = left.patch.value();
    req.left.pre = left.pre;
    return req;
  }
  // => {{ A == 0 && B == 0 && !C.has_value() }}

  // 1.6. `0.0` is equivalent to `=0.0` (i.e., 2.2)
  VersionReq req;
  req.left.op = Comparator::Gte;
  req.left.major = 0;
  req.left.minor = 0;
  req.left.patch = 0;
  req.left.pre = left.pre;

  req.right = Comparator();
  req.right->op = Comparator::Lt;
  req.right->major = 0;
  req.right->minor = 1;
  req.right->patch = 0;
  req.right->pre = left.pre;

  return req;
}

// 2. Exact:
//   2.1. `=A.B.C` is exactly the version `A.B.C`
//   2.2. `=A.B` is equivalent to `>=A.B.0 and <A.(B+1).0`
//   2.3. `=A` is equivalent to `>=A.0.0 and <(A+1).0.0`
static VersionReq canonicalizeExact(const VersionReq& req) noexcept {
  const Comparator& left = req.left;

  if (left.minor.has_value() && left.patch.has_value()) {
    // 2.1. `=A.B.C` is exactly the version A.B.C
    return req;
  } else if (left.minor.has_value()) {
    // 2.2. `=A.B` is equivalent to `>=A.B.0 and <A.(B+1).0`
    VersionReq req;
    req.left.op = Comparator::Gte;
    req.left.major = left.major;
    req.left.minor = left.minor.value();
    req.left.patch = 0;
    req.left.pre = left.pre;

    req.right = Comparator();
    req.right->op = Comparator::Lt;
    req.right->major = left.major;
    req.right->minor = left.minor.value() + 1;
    req.right->patch = 0;
    req.right->pre = left.pre;

    return req;
  } else {
    // 2.3. `=A` is equivalent to `>=A.0.0 and <(A+1).0.0`
    VersionReq req;
    req.left.op = Comparator::Gte;
    req.left.major = left.major;
    req.left.minor = 0;
    req.left.patch = 0;
    req.left.pre = left.pre;

    req.right = Comparator();
    req.right->op = Comparator::Lt;
    req.right->major = left.major + 1;
    req.right->minor = 0;
    req.right->patch = 0;
    req.right->pre = left.pre;

    return req;
  }
}

VersionReq VersionReq::canonicalize() const noexcept {
  if (!left.op.has_value()) { // NoOp
    return canonicalizeNoOp(*this);
  } else if (left.op.value() == Comparator::Exact) {
    return canonicalizeExact(*this);
  }

  VersionReq req = *this;
  req.left = left.canonicalize();
  if (right.has_value()) {
    req.right = right->canonicalize();
  }
  return req;
}

String VersionReq::to_string() const noexcept {
  String result = left.to_string();
  if (right.has_value()) {
    result += " and ";
    result += right->to_string();
  }
  return result;
}

std::ostream& operator<<(std::ostream& os, const VersionReq& req) {
  return os << req.to_string();
}

#ifdef POAC_TEST

#  include "TestUtils.hpp"

// Thanks to:
// https://github.com/dtolnay/semver/blob/b6171889ac7e8f47ec6f12003571bdcc7f737b10/tests/test_version_req.rs

void test_canonicalize_no_op() {
  // 1.1. `A.B.C` (where A > 0) is equivalent to `>=A.B.C and <(A+1).0.0`
  ASSERT_EQ(
      VersionReq::parse("1.2.3").canonicalize().to_string(),
      ">=1.2.3 and <2.0.0"
  );

  // 1.2. `A.B` (where A > 0 & B > 0) is equivalent to `^A.B.0` (i.e., 1.1)
  ASSERT_EQ(
      VersionReq::parse("1.2").canonicalize().to_string(), ">=1.2.0 and <2.0.0"
  );

  // 1.3. `A` is equivalent to `=A` (i.e., 2.3)
  ASSERT_EQ(
      VersionReq::parse("1").canonicalize().to_string(), ">=1.0.0 and <2.0.0"
  );

  // 1.4. `0.B.C` (where B > 0) is equivalent to `>=0.B.C and <0.(B+1).0`
  ASSERT_EQ(
      VersionReq::parse("0.2.3").canonicalize().to_string(),
      ">=0.2.3 and <0.3.0"
  );

  // 1.5. `0.0.C` is equivalent to `=0.0.C` (i.e., 2.1)
  ASSERT_EQ(VersionReq::parse("0.0.3").canonicalize().to_string(), "=0.0.3");

  // 1.6. `0.0` is equivalent to `=0.0` (i.e., 2.2)
  ASSERT_EQ(
      VersionReq::parse("0.0").canonicalize().to_string(), ">=0.0.0 and <0.1.0"
  );
}

void test_canonicalize_exact() {
  // 2.1. `=A.B.C` is exactly the version `A.B.C`
  ASSERT_EQ(VersionReq::parse("=1.2.3").canonicalize().to_string(), "=1.2.3");

  // 2.2. `=A.B` is equivalent to `>=A.B.0 and <A.(B+1).0`
  ASSERT_EQ(
      VersionReq::parse("=1.2").canonicalize().to_string(), ">=1.2.0 and <1.3.0"
  );

  // 2.3. `=A` is equivalent to `>=A.0.0 and <(A+1).0.0`
  ASSERT_EQ(
      VersionReq::parse("=1").canonicalize().to_string(), ">=1.0.0 and <2.0.0"
  );
}

void test_canonicalize_gt() {
  // 3.1. `>A.B.C` is equivalent to `>=A.B.(C+1)`
  ASSERT_EQ(VersionReq::parse(">1.2.3").canonicalize().to_string(), ">=1.2.4");

  // 3.2. `>A.B` is equivalent to `>=A.(B+1).0`
  ASSERT_EQ(VersionReq::parse(">1.2").canonicalize().to_string(), ">=1.3.0");

  // 3.3. `>A` is equivalent to `>=(A+1).0.0`
  ASSERT_EQ(VersionReq::parse(">1").canonicalize().to_string(), ">=2.0.0");
}

void test_canonicalize_gte() {
  // 4.1. `>=A.B.C`
  ASSERT_EQ(VersionReq::parse(">=1.2.3").canonicalize().to_string(), ">=1.2.3");

  // 4.2. `>=A.B` is equivalent to `>=A.B.0`
  ASSERT_EQ(VersionReq::parse(">=1.2").canonicalize().to_string(), ">=1.2.0");

  // 4.3. `>=A` is equivalent to `>=A.0.0`
  ASSERT_EQ(VersionReq::parse(">=1").canonicalize().to_string(), ">=1.0.0");
}

void test_canonicalize_lt() {
  // 5.1. `<A.B.C`
  ASSERT_EQ(VersionReq::parse("<1.2.3").canonicalize().to_string(), "<1.2.3");

  // 5.2. `<A.B` is equivalent to `<A.B.0`
  ASSERT_EQ(VersionReq::parse("<1.2").canonicalize().to_string(), "<1.2.0");

  // 5.3. `<A` is equivalent to `<A.0.0`
  ASSERT_EQ(VersionReq::parse("<1").canonicalize().to_string(), "<1.0.0");
}

void test_canonicalize_lte() {
  // 6.1. `<=A.B.C` is equivalent to `<A.B.(C+1)`
  ASSERT_EQ(VersionReq::parse("<=1.2.3").canonicalize().to_string(), "<1.2.4");

  // 6.2. `<=A.B` is equivalent to `<A.(B+1).0`
  ASSERT_EQ(VersionReq::parse("<=1.2").canonicalize().to_string(), "<1.3.0");

  // 6.3. `<=A` is equivalent to `<(A+1).0.0`
  ASSERT_EQ(VersionReq::parse("<=1").canonicalize().to_string(), "<2.0.0");
}

void test_basic() {
  const auto req = VersionReq::parse("1.0.0");
  // ASSERT_EQ(req.to_string(), ">=1.0.0 and <2.0.0");
}

void test_parse() {
  ASSERT_EXCEPTION(
      VersionReq::parse("\0"), VersionReqException,
      "invalid version requirement:\n"
      "\n"
      "^ expected =, >=, <=, >, <, or version"
  );

  ASSERT_EXCEPTION(
      VersionReq::parse(">= >= 0.0.2"), ComparatorException,
      "invalid comparator:\n"
      ">= >= 0.0.2\n"
      "  ^ expected version"
  );

  ASSERT_EXCEPTION(
      VersionReq::parse(">== 0.0.2"), ComparatorException,
      "invalid comparator:\n"
      ">== 0.0.2\n"
      "   ^ expected version"
  );

  ASSERT_EXCEPTION(
      VersionReq::parse("a.0.0"), VersionReqException,
      "invalid version requirement:\n"
      "a.0.0\n"
      "^ expected =, >=, <=, >, <, or version"
  );

  ASSERT_EXCEPTION(
      VersionReq::parse("1.0.0-"), std::exception,
      "invalid semver:\n"
      "1.0.0-\n"
      "      ^ expected number or identifier"
  );

  ASSERT_EXCEPTION(
      VersionReq::parse(">="), ComparatorException,
      "invalid comparator:\n"
      ">=\n"
      "  ^ expected version"
  );
}

void test_comparator_parse() {
  ASSERT_EXCEPTION(
      Comparator::parse("1.2.3-01"), std::exception,
      "invalid semver:\n"
      "1.2.3-01\n"
      "      ^ invalid leading zero"
  );

  ASSERT_EXCEPTION(
      Comparator::parse("1.2.3+4."), std::exception,
      "invalid semver:\n"
      "1.2.3+4.\n"
      "        ^ expected identifier"
  );

  ASSERT_EXCEPTION(
      Comparator::parse(">"), ComparatorException,
      "invalid comparator:\n"
      ">\n"
      " ^ expected version"
  );

  ASSERT_EXCEPTION(
      Comparator::parse("1."), std::exception,
      "invalid semver:\n"
      "1.\n"
      "  ^ expected number"
  );

  ASSERT_EXCEPTION(
      Comparator::parse("1.*."), std::exception,
      "invalid semver:\n"
      "1.*.\n"
      "  ^ unexpected character: `*`"
  );
}

void test_leading_digit_in_pre_and_build() {
  for (const auto& cmp : {"", "<", "<=", ">", ">="}) {
    // digit then alpha
    ASSERT_NO_EXCEPTION(VersionReq::parse(cmp + "1.2.3-1a"s));
    ASSERT_NO_EXCEPTION(VersionReq::parse(cmp + "1.2.3+1a"s));

    // digit then alpha (leading zero)
    ASSERT_NO_EXCEPTION(VersionReq::parse(cmp + "1.2.3-01a"s));
    ASSERT_NO_EXCEPTION(VersionReq::parse(cmp + "1.2.3+01"s));

    // multiple
    ASSERT_NO_EXCEPTION(VersionReq::parse(cmp + "1.2.3-1+1"s));
    ASSERT_NO_EXCEPTION(VersionReq::parse(cmp + "1.2.3-1-1+1-1-1"s));
    ASSERT_NO_EXCEPTION(VersionReq::parse(cmp + "1.2.3-1a+1a"s));
    ASSERT_NO_EXCEPTION(VersionReq::parse(cmp + "1.2.3-1a-1a+1a-1a-1a"s));
  }
}

int main() {
  REGISTER_TEST(test_canonicalize_no_op);
  REGISTER_TEST(test_canonicalize_exact);
  REGISTER_TEST(test_canonicalize_gt);
  REGISTER_TEST(test_canonicalize_gte);
  REGISTER_TEST(test_canonicalize_lt);
  REGISTER_TEST(test_canonicalize_lte);
  REGISTER_TEST(test_basic);
  REGISTER_TEST(test_parse);
  REGISTER_TEST(test_comparator_parse);
  REGISTER_TEST(test_leading_digit_in_pre_and_build);
}

#endif
