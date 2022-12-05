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

struct exception : public std::exception {
  explicit exception(const String& what) : what_(what) {}
  explicit exception(const char* what) : what_(what) {}
  ~exception() noexcept override = default;
  inline const char* what() const noexcept override { return what_.c_str(); }

  exception(const exception&) = default;
  exception& operator=(const exception&) = default;
  exception(exception&&) noexcept = default;
  exception& operator=(exception&&) noexcept = default;

protected:
  String what_;
};

struct string_error : public cfg::exception {
  explicit string_error(const String& what_)
      : exception("missing terminating '\"' character\n" + what_) {}
  explicit string_error(const char* what_) : string_error(String(what_)) {}
  ~string_error() noexcept override = default;

  string_error(const string_error&) = default;
  string_error& operator=(const string_error&) = default;
  string_error(string_error&&) noexcept = default;
  string_error& operator=(string_error&&) noexcept = default;
};

struct ident_error : public cfg::exception {
public:
  explicit ident_error(const String& what_)
      : exception(
          "cfg expected parenthesis, comma, identifier, or string\n" + what_
      ) {}
  explicit ident_error(const char* what_) : ident_error(String(what_)) {}
  ~ident_error() noexcept override = default;

  ident_error(const ident_error&) = default;
  ident_error& operator=(const ident_error&) = default;
  ident_error(ident_error&&) noexcept = default;
  ident_error& operator=(ident_error&&) noexcept = default;
};

struct operator_error : public cfg::exception {
public:
  explicit operator_error(const String& what_)
      : exception("cfg operator error\n" + what_) {}
  explicit operator_error(const char* what_) : operator_error(String(what_)) {}
  ~operator_error() noexcept override = default;

  operator_error(const operator_error&) = default;
  operator_error& operator=(const operator_error&) = default;
  operator_error(operator_error&&) noexcept = default;
  operator_error& operator=(operator_error&&) noexcept = default;
};

struct expression_error : public cfg::exception {
public:
  explicit expression_error(const String& what_) : exception(what_) {}
  explicit expression_error(const char* what_) : exception(what_) {}
  ~expression_error() noexcept override = default;

  expression_error(const expression_error&) = default;
  expression_error& operator=(const expression_error&) = default;
  expression_error(expression_error&&) noexcept = default;
  expression_error& operator=(expression_error&&) noexcept = default;
};

struct syntax_error : public cfg::exception {
public:
  explicit syntax_error(const String& what_)
      : exception("cfg syntax error\n" + what_) {}
  explicit syntax_error(const char* what_) : syntax_error(String(what_)) {}
  ~syntax_error() noexcept override = default;

  syntax_error(const syntax_error&) = default;
  syntax_error& operator=(const syntax_error&) = default;
  syntax_error(syntax_error&&) noexcept = default;
  syntax_error& operator=(syntax_error&&) noexcept = default;
};

constexpr bool is_ident_start(const char c) noexcept {
  return (c == '_') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

constexpr bool is_ident_rest(const char c) noexcept {
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
      ),
        value() {}
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
  Token& operator=(const Token&) = default;
  Token(Token&&) noexcept = default;
  Token& operator=(Token&&) noexcept = default;
  ~Token() = default;

  inline string_type get_str() const {
    return std::get<string_type>(this->value);
  }
  inline ident get_ident() const { return std::get<ident>(this->value); }

  friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

constexpr Token::Kind to_kind(StringRef kind) {
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
    throw exception("Unexpected to_kind error");
  }
}

String to_string(Token::ident ident);

std::ostream& operator<<(std::ostream& os, const Token& token);

struct Lexer {
  using value_type = StringRef::value_type;
  using size_type = usize;

  StringRef str;
  size_type index;

  explicit Lexer(StringRef str) : str(str), index(0) {}

  inline Option<Token> next() {
    const auto [diff, token] = tokenize(this->index);
    this->step_n(diff);
    return token;
  }

  inline Option<Token> peek() const {
    const auto [diff, token] = tokenize(this->index);
    static_cast<void>(diff);
    return token;
  }

private:
  inline std::pair<size_type, Option<Token>>
  generate_token(size_type index_, const Option<Token>& token) const {
    return {this->diff_step(index_), token};
  }
  inline std::pair<size_type, Option<Token>>
  generate_token(size_type index_, StringRef kind) const {
    return generate_token(index_, Token{to_kind(kind)});
  }

  std::pair<size_type, Option<Token>>
  analyze_two_phrase(size_type index_, const char kind) const;

  std::pair<size_type, Option<Token>> tokenize(size_type index_) const;

  void step(size_type& index_) const noexcept;
  void step_n(const size_type n) noexcept;

  inline size_type diff_step(const size_type index_) const noexcept {
    return index_ - this->index;
  }

  inline value_type one(const size_type index_) const noexcept {
    return this->str[index_];
  }

  std::pair<size_type, Token> string(size_type index_) const;

  std::pair<size_type, Token> ident(size_type index_) const;

  Option<Token::ident> to_ident(StringRef s) const noexcept;
};

enum class compiler {
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
  Cfg& operator=(const Cfg&) = default;
  Cfg(Cfg&&) noexcept = default;
  Cfg& operator=(Cfg&&) noexcept = default;
  ~Cfg() = default;

private:
  Ident from_token_ident(Token::ident ident) const;
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
  CfgExpr& operator=(const CfgExpr&) = delete;
  CfgExpr(CfgExpr&&) noexcept = default;
  CfgExpr& operator=(CfgExpr&&) noexcept = default;
  ~CfgExpr() = default;

  bool match() const;

private:
  bool match(const Cfg& c) const;
};

struct Parser {
  Lexer lexer;

  explicit Parser(StringRef str) : lexer(str) {}

  CfgExpr expr();

  Cfg cfg();

private:
  [[noreturn]] void throw_operator_error(const usize index, Cfg::Op op) const;

  Cfg cfg_str(const usize index, Token::ident ident, Cfg::Op op);

  Cfg cfg_str(Token::ident ident, Cfg::Op op);

  bool r_try(Token::Kind kind);

  void eat_left_paren(Token::ident prev);
  void eat_right_paren();
};

inline CfgExpr parse(StringRef s) { return Parser(s).expr(); }

} // end namespace poac::util::cfg

#endif // POAC_UTIL_CFG_HPP_
