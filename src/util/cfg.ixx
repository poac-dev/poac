module;

// std
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

// external
#include <boost/predef.h> // NOLINT(build/include_order)

export module poac.util.cfg;

import poac.util.rustify;

export namespace poac::util::cfg {

struct Exception : public std::exception {
  explicit Exception(String what) : what_(std::move(what)) {}
  explicit Exception(const char* what) : what_(what) {}
  ~Exception() noexcept override = default;
  [[nodiscard]] inline auto what() const noexcept -> const char* override {
    return what_.c_str();
  }

  Exception(const Exception&) = default;
  auto operator=(const Exception&) -> Exception& = default;
  Exception(Exception&&) noexcept = default;
  auto operator=(Exception&&) noexcept -> Exception& = default;

private:
  String what_;
};

struct StringError : public cfg::Exception {
  explicit StringError(const String& what_)
      : Exception("missing terminating '\"' character\n" + what_) {}
  explicit StringError(const char* what_) : StringError(String(what_)) {}
  ~StringError() noexcept override = default;

  StringError(const StringError&) = default;
  auto operator=(const StringError&) -> StringError& = default;
  StringError(StringError&&) noexcept = default;
  auto operator=(StringError&&) noexcept -> StringError& = default;
};

struct IdentError : public cfg::Exception {
public:
  explicit IdentError(const String& what_)
      : Exception(
          "cfg expected parenthesis, comma, identifier, or string\n" + what_
      ) {}
  explicit IdentError(const char* what_) : IdentError(String(what_)) {}
  ~IdentError() noexcept override = default;

  IdentError(const IdentError&) = default;
  auto operator=(const IdentError&) -> IdentError& = default;
  IdentError(IdentError&&) noexcept = default;
  auto operator=(IdentError&&) noexcept -> IdentError& = default;
};

struct OperatorError : public cfg::Exception {
public:
  explicit OperatorError(const String& what_)
      : Exception("cfg operator error\n" + what_) {}
  explicit OperatorError(const char* what_) : OperatorError(String(what_)) {}
  ~OperatorError() noexcept override = default;

  OperatorError(const OperatorError&) = default;
  auto operator=(const OperatorError&) -> OperatorError& = default;
  OperatorError(OperatorError&&) noexcept = default;
  auto operator=(OperatorError&&) noexcept -> OperatorError& = default;
};

struct ExpressionError : public cfg::Exception {
public:
  explicit ExpressionError(const String& what_) : Exception(what_) {}
  explicit ExpressionError(const char* what_) : Exception(what_) {}
  ~ExpressionError() noexcept override = default;

  ExpressionError(const ExpressionError&) = default;
  auto operator=(const ExpressionError&) -> ExpressionError& = default;
  ExpressionError(ExpressionError&&) noexcept = default;
  auto operator=(ExpressionError&&) noexcept -> ExpressionError& = default;
};

struct SyntaxError : public cfg::Exception {
public:
  explicit SyntaxError(const String& what_)
      : Exception("cfg syntax error\n" + what_) {}
  explicit SyntaxError(const char* what_) : SyntaxError(String(what_)) {}
  ~SyntaxError() noexcept override = default;

  SyntaxError(const SyntaxError&) = default;
  auto operator=(const SyntaxError&) -> SyntaxError& = default;
  SyntaxError(SyntaxError&&) noexcept = default;
  auto operator=(SyntaxError&&) noexcept -> SyntaxError& = default;
};

constexpr auto is_ident_start(const char c) noexcept -> bool {
  return (c == '_') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

constexpr auto is_ident_rest(const char c) noexcept -> bool {
  return is_ident_start(c) || ('0' <= c && c <= '9');
}

struct Token {
  enum Kind {
    /// `(`
    LeftParen,
    /// `)`
    RightParen,
    /// `,`
    Comma,
    /// `=`
    Equals,
    /// `>`
    Gt,
    /// `>=`
    GtEq,
    /// `<`
    Lt,
    /// `<=`
    LtEq,
    String,
    Ident,
  };

  // NOLINTNEXTLINE(readability-identifier-naming)
  enum class ident {
    cfg,
    not_,
    all,
    any,
    compiler,
    arch,
    feature,
    os,
    os_version,
    platform,
  };

  Kind kind;
  using StringType = StringRef;
  std::variant<std::monostate, StringType, ident> value;

  explicit Token(Kind k)
      : kind(
          k != Kind::String && k != Kind::Ident
              ? k
              : throw std::invalid_argument("poac::util::cfg::Token")
      ) {}
  Token(Kind k, StringType s)
      : kind(
          k == Kind::String
              ? k
              : throw std::invalid_argument("poac::util::cfg::Token")
      ),
        value(s) {}
  Token(Kind k, ident i)
      : kind(
          k == Kind::Ident
              ? k
              : throw std::invalid_argument("poac::util::cfg::Token")
      ),
        value(i) {}

  Token() = delete;
  Token(const Token&) = default;
  auto operator=(const Token&) -> Token& = default;
  Token(Token&&) noexcept = default;
  auto operator=(Token&&) noexcept -> Token& = default;
  ~Token() = default;

  [[nodiscard]] inline auto get_str() const -> StringType {
    return std::get<StringType>(this->value);
  }
  [[nodiscard]] inline auto get_ident() const -> ident {
    return std::get<ident>(this->value);
  }

  friend auto operator<<(std::ostream& os, const Token& token) -> std::ostream&;
};

constexpr auto to_kind(StringRef kind) -> Token::Kind {
  if (kind == "(") {
    return Token::LeftParen;
  } else if (kind == ")") {
    return Token::RightParen;
  } else if (kind == ",") {
    return Token::Comma;
  } else if (kind == "=") {
    return Token::Equals;
  } else if (kind == ">") {
    return Token::Gt;
  } else if (kind == ">=") {
    return Token::GtEq;
  } else if (kind == "<") {
    return Token::Lt;
  } else if (kind == "<=") {
    return Token::LtEq;
  } else {
    throw Exception("Unexpected to_kind error");
  }
}

auto to_string(Token::ident ident) -> String {
  switch (ident) {
    case Token::ident::cfg:
      return "cfg";
    case Token::ident::not_:
      return "not";
    case Token::ident::all:
      return "all";
    case Token::ident::any:
      return "any";
    case Token::ident::compiler:
      return "compiler";
    case Token::ident::arch:
      return "arch";
    case Token::ident::feature:
      return "feature";
    case Token::ident::os:
      return "os";
    case Token::ident::os_version:
      return "os_version";
    case Token::ident::platform:
      return "platform";
    default:
      __builtin_unreachable();
  }
}

auto operator<<(std::ostream& os, const Token& token) -> std::ostream& {
  switch (token.kind) {
    case Token::LeftParen:
      return (os << "left_paren: (");
    case Token::RightParen:
      return (os << "right_paren: )");
    case Token::Comma:
      return (os << "comma: ,");
    case Token::Equals:
      return (os << "equals: =");
    case Token::Gt:
      return (os << "gt: >");
    case Token::GtEq:
      return (os << "gteq: >=");
    case Token::Lt:
      return (os << "lt: <");
    case Token::LtEq:
      return (os << "lteq: <=");
    case Token::String:
      return (os << "string: " << token.get_str());
    case Token::Ident:
      return (os << "ident: " << to_string(token.get_ident()));
    default:
      __builtin_unreachable();
  }
}

struct Lexer {
  using ValueType = StringRef::value_type;
  using SizeType = usize;

  StringRef str;
  SizeType index{0};

  explicit Lexer(StringRef str) : str(str) {}

  inline auto next() -> Option<Token> {
    const auto [diff, token] = tokenize(this->index);
    this->step_n(diff);
    return token;
  }

  [[nodiscard]] inline auto peek() const -> Option<Token> {
    const auto [diff, token] = tokenize(this->index);
    static_cast<void>(diff);
    return token;
  }

private:
  [[nodiscard]] inline auto
  generate_token(SizeType index_, const Option<Token>& token) const
      -> std::pair<SizeType, Option<Token>> {
    return {this->diff_step(index_), token};
  }
  [[nodiscard]] inline auto
  generate_token(SizeType index_, StringRef kind) const
      -> std::pair<SizeType, Option<Token>> {
    return generate_token(index_, Token{to_kind(kind)});
  }

  [[nodiscard]] auto analyze_two_phrase(SizeType index_, char kind) const
      -> std::pair<SizeType, Option<Token>>;

  [[nodiscard]] auto tokenize(SizeType index_) const
      -> std::pair<SizeType, Option<Token>>;

  void step(SizeType& index_) const noexcept;
  void step_n(SizeType n) noexcept;

  [[nodiscard]] inline auto diff_step(const SizeType index_) const noexcept
      -> SizeType {
    return index_ - this->index;
  }

  [[nodiscard]] inline auto one(const SizeType index_) const noexcept
      -> ValueType {
    return this->str[index_];
  }

  [[nodiscard]] auto string(SizeType index_) const
      -> std::pair<SizeType, Token>;

  [[nodiscard]] auto ident(SizeType index_) const -> std::pair<SizeType, Token>;

  static auto to_ident(StringRef s) noexcept -> Option<Token::ident>;
};

auto Lexer::analyze_two_phrase(SizeType index_, const char kind) const
    -> std::pair<Lexer::SizeType, Option<Token>> {
  if (this->one(index_) == '=') {
    this->step(index_);
    return generate_token(index_, String{kind} + '=');
  } else {
    return generate_token(index_, String{kind});
  }
}

auto Lexer::tokenize(SizeType index_) const
    -> std::pair<Lexer::SizeType, Option<Token>> {
  if (index_ >= this->str.size()) {
    return generate_token(index_, None);
  }
  const char one = this->one(index_);
  switch (one) {
    case ' ':
      do {
        this->step(index_);
      } while (this->one(index_) == ' ');
      return tokenize(index_);
    case '(':
    case ')':
    case ',':
      [[fallthrough]];
    case '=':
      this->step(index_);
      return generate_token(index_, String{one});
    case '>':
      [[fallthrough]];
    case '<':
      this->step(index_);
      return analyze_two_phrase(index_, one);
    case '"':
      return string(index_);
    default:
      return ident(index_);
  }
}

void Lexer::step(SizeType& index_) const noexcept {
  if (index_ < this->str.size()) {
    ++index_;
  }
}
void Lexer::step_n(const SizeType n) noexcept {
  for (SizeType i = 0; i < n; ++i) {
    this->step(this->index);
  }
}

auto Lexer::string(SizeType index_) const -> std::pair<Lexer::SizeType, Token> {
  this->step(index_);
  const SizeType start = index_;
  while (this->one(index_) != '"') {
    this->step(index_);
    if (index_ >= this->str.size()) {
      String msg;
      msg += String(start - 1, ' ');
      msg += "^";
      msg += String(this->str.size() - start, '-');
      msg += " unterminated string";
      throw cfg::StringError(String(this->str) + "\n" + msg);
    }
  }
  const StringRef s = this->str.substr(start, index_ - start);
  this->step(index_);
  return {this->diff_step(index_), Token{Token::String, s}};
}

auto Lexer::ident(SizeType index_) const -> std::pair<Lexer::SizeType, Token> {
  if (!is_ident_start(this->one(index_))) {
    String msg;
    msg += String(index_, ' ');
    msg += "^ unexpected character";
    throw cfg::IdentError(String(this->str) + "\n" + msg);
  }
  const SizeType start = index_;
  this->step(index_);
  while (is_ident_rest(this->one(index_))) {
    this->step(index_);
  }

  const StringRef s = this->str.substr(start, index_ - start);
  if (const auto ident = to_ident(s)) {
    return {this->diff_step(index_), Token{Token::Ident, ident.value()}};
  } else {
    String msg;
    msg += String(start, ' ');
    msg += "^";
    msg += String(index_ - start - 1, '-');
    msg += " unknown identify";
    throw cfg::IdentError(String(this->str) + "\n" + msg);
  }
}

auto Lexer::to_ident(StringRef s) noexcept -> Option<Token::ident> {
  if (s == "cfg") {
    return Token::ident::cfg;
  } else if (s == "not") {
    return Token::ident::not_;
  } else if (s == "all") {
    return Token::ident::all;
  } else if (s == "any") {
    return Token::ident::any;
  } else if (s == "compiler") {
    return Token::ident::compiler;
  } else if (s == "arch") {
    return Token::ident::arch;
  } else if (s == "feature") {
    return Token::ident::feature;
  } else if (s == "os") {
    return Token::ident::os;
  } else if (s == "platform") {
    return Token::ident::platform;
  } else if (s == "os_version") {
    return Token::ident::os_version;
  } else {
    return None;
  }
}

enum class Compiler {
  gcc,
  clang,
  apple_clang,
  msvc,
  icc,
};

struct Cfg {
  enum class Ident {
    compiler,
    arch,
    feature,
    os,
    platform,
    os_version,
  };
  enum class Op {
    Equals,
    Gt,
    GtEq,
    Lt,
    LtEq,
  };

  Ident key;
  Op op;
  StringRef value;

  Cfg(Token::ident key, Op op, StringRef value)
      : key(from_token_ident(key)), op(op), value(value) {}

  Cfg() = delete;
  Cfg(const Cfg&) = default;
  auto operator=(const Cfg&) -> Cfg& = default;
  Cfg(Cfg&&) noexcept = default;
  auto operator=(Cfg&&) noexcept -> Cfg& = default;
  ~Cfg() = default;

private:
  static auto from_token_ident(Token::ident ident) -> Ident;
};

struct CfgExpr {
  enum Kind {
    cfg,
    not_,
    all,
    any,
    value,
  };

  using NullType = std::monostate;
  using ExprType = std::unique_ptr<CfgExpr>;
  using ExprListType = Vec<CfgExpr>;
  using VariantType = std::variant<NullType, ExprType, ExprListType, Cfg>;

  Kind kind;
  VariantType expr;

  CfgExpr(Kind kind, ExprType&& expr)
      : kind(
          kind == Kind::not_ || kind == Kind::cfg
              ? kind
              : throw std::invalid_argument("poac::util::cfg::CfgExpr")
      ),
        expr(std::move(expr)) {}
  CfgExpr(Kind kind, ExprListType&& expr)
      : kind(
          kind == Kind::all || kind == Kind::any
              ? kind
              : throw std::invalid_argument("poac::util::cfg::CfgExpr")
      ),
        expr(std::move(expr)) {}
  CfgExpr(Kind kind, const Cfg& c)
      : kind(
          kind == Kind::value
              ? kind
              : throw std::invalid_argument("poac::util::cfg::CfgExpr")
      ),
        expr(c) {}

  CfgExpr() = delete;
  CfgExpr(const CfgExpr&) = delete;
  auto operator=(const CfgExpr&) -> CfgExpr& = delete;
  CfgExpr(CfgExpr&&) noexcept = default;
  auto operator=(CfgExpr&&) noexcept -> CfgExpr& = default;
  ~CfgExpr() = default;

  [[nodiscard]] auto match() const -> bool;

private:
  static auto match(const Cfg& c) -> bool;
};

auto Cfg::from_token_ident(Token::ident ident) -> Cfg::Ident {
  switch (ident) {
    case Token::ident::compiler:
      return Ident::compiler;
    case Token::ident::arch:
      return Ident::arch;
    case Token::ident::feature:
      return Ident::feature;
    case Token::ident::os:
      return Ident::os;
    case Token::ident::platform:
      return Ident::platform;
    case Token::ident::os_version:
      return Ident::os_version;
    default:
      throw std::invalid_argument("poac::util::cfg::Cfg");
  }
}

auto CfgExpr::match() const -> bool {
  switch (this->kind) {
    case Kind::cfg:
      return std::get<ExprType>(this->expr)->match();
    case Kind::not_:
      return !(std::get<ExprType>(this->expr)->match());
    case Kind::all: {
      bool res = true;
      for (const CfgExpr& c : std::get<ExprListType>(this->expr)) {
        res &= c.match();
      }
      return res;
    }
    case Kind::any: {
      bool res = false;
      for (const CfgExpr& c : std::get<ExprListType>(this->expr)) {
        res |= c.match();
      }
      return res;
    }
    case Kind::value:
      return match(std::get<Cfg>(this->expr));
    default:
      __builtin_unreachable();
  }
}

auto CfgExpr::match(const Cfg& c) -> bool {
  switch (c.key) {
    case Cfg::Ident::compiler:
      return false;
    case Cfg::Ident::arch:
#if BOOST_ARCH_ALPHA
      return c.value == "alpha";
#elif BOOST_ARCH_ARM
      return c.value == "arm";
#elif BOOST_ARCH_BLACKFIN
      return c.value == "blackfin";
#elif BOOST_ARCH_CONVEX
      return c.value == "convex";
#elif BOOST_ARCH_IA64
      return c.value == "ia64";
#elif BOOST_ARCH_M68K
      return c.value == "m68k";
#elif BOOST_ARCH_MIPS
      return c.value == "mips";
#elif BOOST_ARCH_SYS390
      return c.value == "s390x";
#elif BOOST_ARCH_X86_32
      return c.value == "x86_32" || c.value == "x86";
#elif BOOST_ARCH_X86_64
      return c.value == "x86_64" || c.value == "x86";
#else
      return false;
#endif
    case Cfg::Ident::feature:
//        aes,
#ifdef __AVX__
      return c.value == "avx";
#elif defined(__AVX2__)
      return c.value == "avx2";
#elif defined(__BMI1__)
      return c.value == "bmi1";
#elif defined(__BMI2__)
      return c.value == "bmi2";
#elif defined(__FMA__)
      return c.value == "fma";
//        fxsr,
//        lzcnt,
//        pclmulqdq,
//        popcnt,
//        rdrand,
//        rdseed,
//        sha,
//        sse,
//        sse2,
//        sse3,
//        sse4.1,
//        sse4.2,
//        ssse3,
//        xsave,
//        xsavec,
//        xsaveopt,
//        xsaves,
#else
      return false;
#endif
    case Cfg::Ident::os:
#if BOOST_OS_WINDOWS
      return c.value == "windows";
#elif BOOST_OS_MACOS
      return c.value == "macos" || c.value == "unix";
#elif BOOST_OS_IOS
      return c.value == "ios";
#elif BOOST_OS_LINUX
      return c.value == "linux" || c.value == "unix";
#elif BOOST_OS_CYGWIN
      return c.value == "cygwin" || c.value == "unix";
#elif BOOST_OS_ANDROID
      return c.value == "android";
#elif BOOST_OS_BSD_FREE
      return c.value == "freebsd";
#elif BOOST_OS_BSD_DRAGONFLY
      return c.value == "dragonfly";
#elif BOOST_OS_BSD_OPEN
      return c.value == "openbsd";
#elif BOOST_OS_BSD_NET
      return c.value == "netbsd";
#else
      return false;
#endif
    case Cfg::Ident::platform:
#if BOOST_PLAT_MINGW
      return c.value == "mingw";
#else
      return false;
#endif
    case Cfg::Ident::os_version:
      return false;
    default:
      __builtin_unreachable();
  }
}

struct Parser {
  Lexer lexer;

  explicit Parser(StringRef str) : lexer(str) {}

  auto expr() -> CfgExpr;

  auto cfg() -> Cfg;

private:
  [[noreturn]] void throw_operator_error(usize index, Cfg::Op op) const;

  auto cfg_str(usize index, Token::ident ident, Cfg::Op op) -> Cfg;

  auto cfg_str(Token::ident ident, Cfg::Op op) -> Cfg;

  auto r_try(Token::Kind kind) -> bool;

  void eat_left_paren(Token::ident prev);
  void eat_right_paren();
};

auto Parser::expr() -> CfgExpr {
  if (const auto token = lexer.peek(); !token.has_value()) {
    throw cfg::ExpressionError("expected start of a cfg expression");
  } else if (token->kind == Token::Ident) {
    if (token->get_ident() == Token::ident::all
        || token->get_ident() == Token::ident::any) {
      this->lexer.next();
      this->eat_left_paren(token->get_ident());
      Vec<CfgExpr> e;
      do { // `all` and `any` need at least one expression.
        e.emplace_back(this->expr());
        if (!this->r_try(Token::Comma)) {
          this->eat_right_paren();
          break;
        }
      } while (!r_try(Token::RightParen));
      if (token->get_ident() == Token::ident::all) {
        return CfgExpr{CfgExpr::all, std::move(e)};
      } else {
        return CfgExpr{CfgExpr::any, std::move(e)};
      }
    } else if (token->get_ident() == Token::ident::not_ || token->get_ident() == Token::ident::cfg) {
      this->lexer.next();
      this->eat_left_paren(token->get_ident());
      CfgExpr&& e = this->expr();
      this->eat_right_paren();
      if (token->get_ident() == Token::ident::not_) {
        return CfgExpr{CfgExpr::not_, std::make_unique<CfgExpr>(std::move(e))};
      } else {
        return CfgExpr{CfgExpr::cfg, std::make_unique<CfgExpr>(std::move(e))};
      }
    }
  }
  return CfgExpr{CfgExpr::value, this->cfg()};
}

auto Parser::cfg() -> Cfg {
  const usize index = lexer.index;
  if (const auto token = lexer.next(); !token.has_value()) {
    String msg = String(index + 1, ' ');
    msg += " ^ expected operator, but cfg expression ended";
    throw cfg::SyntaxError(String(lexer.str) + "\n" + msg);
  } else if (token->kind == Token::Ident) {
    if (this->r_try(Token::Equals)) {
      return this->cfg_str(token->get_ident(), Cfg::Op::Equals);
    } else if (this->r_try(Token::Gt)) {
      return this->cfg_str(lexer.index, token->get_ident(), Cfg::Op::Gt);
    } else if (this->r_try(Token::GtEq)) {
      return this->cfg_str(lexer.index, token->get_ident(), Cfg::Op::GtEq);
    } else if (this->r_try(Token::Lt)) {
      return this->cfg_str(lexer.index, token->get_ident(), Cfg::Op::Lt);
    } else if (this->r_try(Token::LtEq)) {
      return this->cfg_str(lexer.index, token->get_ident(), Cfg::Op::LtEq);
    }
  }
  String msg = String(lexer.index + 1, ' ');
  msg += "^ expected operator";
  throw cfg::SyntaxError(String(lexer.str) + "\n" + msg);
}

[[noreturn]] void
Parser::throw_operator_error(const usize index, Cfg::Op op) const {
  String msg;
  if (op == Cfg::Op::Gt || op == Cfg::Op::Lt) {
    msg += String(index - 1, ' ');
    msg += "^";
  } else if (op == Cfg::Op::GtEq || op == Cfg::Op::LtEq) {
    msg += String(index - 2, ' ');
    msg += "^-";
  }
  msg += " cannot be specified except os_version";
  throw cfg::OperatorError(String(lexer.str) + "\n" + msg);
}

auto Parser::cfg_str(const usize index, Token::ident ident, Cfg::Op op) -> Cfg {
  const Cfg c = this->cfg_str(ident, op);
  if (ident != Token::ident::os_version) {
    this->throw_operator_error(index, op);
  }
  return c;
}

auto Parser::cfg_str(Token::ident ident, Cfg::Op op) -> Cfg {
  const usize index = lexer.index;
  if (const auto t = lexer.next()) {
    if (t->kind == Token::String) {
      return {ident, op, t->get_str()};
    } else {
      String msg = String(index + 1, ' ');
      msg += "^";
      const i32 range = lexer.index - index - 2;
      msg += String(range < 0 ? 0 : range, '-');
      msg += " expected a string";
      throw cfg::SyntaxError(String(lexer.str) + "\n" + msg);
    }
  } else {
    String msg = String(index, ' ');
    msg += "^ expected a string, but cfg expression ended";
    throw cfg::SyntaxError(String(lexer.str) + "\n" + msg);
  }
}

auto Parser::r_try(Token::Kind kind) -> bool {
  if (const auto token = lexer.peek()) {
    if (token->kind == kind) {
      this->lexer.next();
      return true;
    }
  }
  return false;
}

void Parser::eat_left_paren(Token::ident prev) {
  const usize index = lexer.index;
  if (const auto token = lexer.next()) {
    if (token->kind != Token::LeftParen) {
      String msg = String(index, ' ');
      msg += "^ excepted '(' after `" + to_string(prev) + "`";
      throw cfg::SyntaxError(String(lexer.str) + "\n" + msg);
    }
  } else {
    String msg = String(index, ' ');
    msg += "^ expected '(', but cfg expression ended";
    throw cfg::SyntaxError(String(lexer.str) + "\n" + msg);
  }
}
void Parser::eat_right_paren() {
  const usize index = lexer.index;
  if (const auto token = lexer.next()) {
    if (token->kind != Token::RightParen) {
      String msg = String(index, ' ');
      msg += "^";
      msg += String(lexer.index - index - 1, '-');
      msg += " excepted ')'";
      throw cfg::SyntaxError(String(lexer.str) + "\n" + msg);
    }
  } else {
    String msg = String(index, ' ');
    msg += "^ expected ')', but cfg expression ended";
    throw cfg::SyntaxError(String(lexer.str) + "\n" + msg);
  }
}

inline auto parse(StringRef s) -> CfgExpr { return Parser(s).expr(); }

} // end namespace poac::util::cfg
