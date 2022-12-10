#ifndef POAC_UTIL_CFG_HPP_
#define POAC_UTIL_CFG_HPP_

// std
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

// internal
#include "poac/poac.hpp"

namespace poac::util::cfg {

struct Exception : public std::exception {
  explicit Exception(String what) : what_(std::move(what)) {}
  explicit Exception(const char* what) : what_(what) {}
  ~Exception() noexcept override = default;
  [[nodiscard]] inline Fn what() const noexcept -> const char* override {
    return what_.c_str();
  }

  Exception(const Exception&) = default;
  Fn operator=(const Exception&)->Exception& = default;
  Exception(Exception&&) noexcept = default;
  Fn operator=(Exception&&) noexcept -> Exception& = default;

private:
  String what_;
};

struct StringError : public cfg::Exception {
  explicit StringError(const String& what_)
      : Exception("missing terminating '\"' character\n" + what_) {}
  explicit StringError(const char* what_) : StringError(String(what_)) {}
  ~StringError() noexcept override = default;

  StringError(const StringError&) = default;
  Fn operator=(const StringError&)->StringError& = default;
  StringError(StringError&&) noexcept = default;
  Fn operator=(StringError&&) noexcept -> StringError& = default;
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
  Fn operator=(const IdentError&)->IdentError& = default;
  IdentError(IdentError&&) noexcept = default;
  Fn operator=(IdentError&&) noexcept -> IdentError& = default;
};

struct OperatorError : public cfg::Exception {
public:
  explicit OperatorError(const String& what_)
      : Exception("cfg operator error\n" + what_) {}
  explicit OperatorError(const char* what_) : OperatorError(String(what_)) {}
  ~OperatorError() noexcept override = default;

  OperatorError(const OperatorError&) = default;
  Fn operator=(const OperatorError&)->OperatorError& = default;
  OperatorError(OperatorError&&) noexcept = default;
  Fn operator=(OperatorError&&) noexcept -> OperatorError& = default;
};

struct ExpressionError : public cfg::Exception {
public:
  explicit ExpressionError(const String& what_) : Exception(what_) {}
  explicit ExpressionError(const char* what_) : Exception(what_) {}
  ~ExpressionError() noexcept override = default;

  ExpressionError(const ExpressionError&) = default;
  Fn operator=(const ExpressionError&)->ExpressionError& = default;
  ExpressionError(ExpressionError&&) noexcept = default;
  Fn operator=(ExpressionError&&) noexcept -> ExpressionError& = default;
};

struct SyntaxError : public cfg::Exception {
public:
  explicit SyntaxError(const String& what_)
      : Exception("cfg syntax error\n" + what_) {}
  explicit SyntaxError(const char* what_) : SyntaxError(String(what_)) {}
  ~SyntaxError() noexcept override = default;

  SyntaxError(const SyntaxError&) = default;
  Fn operator=(const SyntaxError&)->SyntaxError& = default;
  SyntaxError(SyntaxError&&) noexcept = default;
  Fn operator=(SyntaxError&&) noexcept -> SyntaxError& = default;
};

constexpr Fn is_ident_start(const char c) noexcept -> bool {
  return (c == '_') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

constexpr Fn is_ident_rest(const char c) noexcept -> bool {
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
  using string_type = StringRef;
  std::variant<std::monostate, string_type, ident> value;

  explicit Token(Kind k)
      : kind(
          k != Kind::String && k != Kind::Ident
              ? k
              : throw std::invalid_argument("poac::util::cfg::Token")
      ) {}
  Token(Kind k, string_type s)
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
  Fn operator=(const Token&)->Token& = default;
  Token(Token&&) noexcept = default;
  Fn operator=(Token&&) noexcept -> Token& = default;
  ~Token() = default;

  [[nodiscard]] inline Fn get_str() const->string_type {
    return std::get<string_type>(this->value);
  }
  [[nodiscard]] inline Fn get_ident() const->ident {
    return std::get<ident>(this->value);
  }

  friend Fn operator<<(std::ostream& os, const Token& token)->std::ostream&;
};

constexpr Fn to_kind(StringRef kind)->Token::Kind {
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

Fn to_string(Token::ident ident)->String;

Fn operator<<(std::ostream& os, const Token& token)->std::ostream&;

struct Lexer {
  using value_type = StringRef::value_type;
  using size_type = usize;

  StringRef str;
  size_type index{0};

  explicit Lexer(StringRef str) : str(str) {}

  inline Fn next()->Option<Token> {
    const auto [diff, token] = tokenize(this->index);
    this->step_n(diff);
    return token;
  }

  [[nodiscard]] inline Fn peek() const->Option<Token> {
    const auto [diff, token] = tokenize(this->index);
    static_cast<void>(diff);
    return token;
  }

private:
  [[nodiscard]] inline Fn generate_token(
      size_type index_, const Option<Token>& token
  ) const->std::pair<size_type, Option<Token>> {
    return {this->diff_step(index_), token};
  }
  [[nodiscard]] inline Fn generate_token(size_type index_, StringRef kind) const
      ->std::pair<size_type, Option<Token>> {
    return generate_token(index_, Token{to_kind(kind)});
  }

  [[nodiscard]] Fn analyze_two_phrase(size_type index_, char kind) const
      ->std::pair<size_type, Option<Token>>;

  [[nodiscard]] Fn tokenize(size_type index_
  ) const->std::pair<size_type, Option<Token>>;

  void step(size_type& index_) const noexcept;
  void step_n(size_type n) noexcept;

  [[nodiscard]] inline Fn diff_step(const size_type index_) const noexcept
      -> size_type {
    return index_ - this->index;
  }

  [[nodiscard]] inline Fn one(const size_type index_) const noexcept
      -> value_type {
    return this->str[index_];
  }

  [[nodiscard]] Fn string(size_type index_) const->std::pair<size_type, Token>;

  [[nodiscard]] Fn ident(size_type index_) const->std::pair<size_type, Token>;

  static Fn to_ident(StringRef s) noexcept -> Option<Token::ident>;
};

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
  Fn operator=(const Cfg&)->Cfg& = default;
  Cfg(Cfg&&) noexcept = default;
  Fn operator=(Cfg&&) noexcept -> Cfg& = default;
  ~Cfg() = default;

private:
  static Fn from_token_ident(Token::ident ident)->Ident;
};

struct CfgExpr {
  enum Kind {
    cfg,
    not_,
    all,
    any,
    value,
  };

  using null_type = std::monostate;
  using expr_type = std::unique_ptr<CfgExpr>;
  using expr_list_type = Vec<CfgExpr>;
  using variant_type = std::variant<null_type, expr_type, expr_list_type, Cfg>;

  Kind kind;
  variant_type expr;

  CfgExpr(Kind kind, expr_type&& expr)
      : kind(
          kind == Kind::not_ || kind == Kind::cfg
              ? kind
              : throw std::invalid_argument("poac::util::cfg::CfgExpr")
      ),
        expr(std::move(expr)) {}
  CfgExpr(Kind kind, expr_list_type&& expr)
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
  Fn operator=(const CfgExpr&)->CfgExpr& = delete;
  CfgExpr(CfgExpr&&) noexcept = default;
  Fn operator=(CfgExpr&&) noexcept -> CfgExpr& = default;
  ~CfgExpr() = default;

  [[nodiscard]] Fn match() const->bool;

private:
  static Fn match(const Cfg& c)->bool;
};

struct Parser {
  Lexer lexer;

  explicit Parser(StringRef str) : lexer(str) {}

  Fn expr()->CfgExpr;

  Fn cfg()->Cfg;

private:
  [[noreturn]] void throw_operator_error(usize index, Cfg::Op op) const;

  Fn cfg_str(usize index, Token::ident ident, Cfg::Op op)->Cfg;

  Fn cfg_str(Token::ident ident, Cfg::Op op)->Cfg;

  Fn r_try(Token::Kind kind)->bool;

  void eat_left_paren(Token::ident prev);
  void eat_right_paren();
};

inline Fn parse(StringRef s)->CfgExpr { return Parser(s).expr(); }

} // end namespace poac::util::cfg

#endif // POAC_UTIL_CFG_HPP_
