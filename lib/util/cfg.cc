// external
#include <boost/predef.h> // NOLINT(build/include_order)

// internal
#include "poac/util/cfg.hpp"

namespace poac::util::cfg {

String
to_string(Token::ident ident) {
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
      unreachable();
  }
}

std::ostream&
operator<<(std::ostream& os, const Token& token) {
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
      unreachable();
  }
}

std::pair<Lexer::size_type, Option<Token>>
Lexer::analyze_two_phrase(size_type index_, const char kind) const {
  if (this->one(index_) == '=') {
    this->step(index_);
    return generate_token(index_, String{kind} + '=');
  } else {
    return generate_token(index_, String{kind});
  }
}

std::pair<Lexer::size_type, Option<Token>>
Lexer::tokenize(size_type index_) const {
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
      [[fallthrough]];
    case ')':
      [[fallthrough]];
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

void
Lexer::step(size_type& index_) const noexcept {
  if (index_ < this->str.size()) {
    ++index_;
  }
}
void
Lexer::step_n(const size_type n) noexcept {
  for (size_type i = 0; i < n; ++i) {
    this->step(this->index);
  }
}

std::pair<Lexer::size_type, Token>
Lexer::string(size_type index_) const {
  this->step(index_);
  const size_type start = index_;
  while (this->one(index_) != '"') {
    this->step(index_);
    if (index_ >= this->str.size()) {
      String msg;
      msg += String(start - 1, ' ');
      msg += "^";
      msg += String(this->str.size() - start, '-');
      msg += " unterminated string";
      throw cfg::string_error(String(this->str) + "\n" + msg);
    }
  }
  StringRef s = this->str.substr(start, index_ - start);
  this->step(index_);
  return {this->diff_step(index_), Token{Token::String, s}};
}

std::pair<Lexer::size_type, Token>
Lexer::ident(size_type index_) const {
  if (!is_ident_start(this->one(index_))) {
    String msg;
    msg += String(index_, ' ');
    msg += "^ unexpected character";
    throw cfg::ident_error(String(this->str) + "\n" + msg);
  }
  const size_type start = index_;
  this->step(index_);
  while (is_ident_rest(this->one(index_))) {
    this->step(index_);
  }

  StringRef s = this->str.substr(start, index_ - start);
  if (const auto ident = to_ident(s)) {
    return {this->diff_step(index_), Token{Token::Ident, ident.value()}};
  } else {
    String msg;
    msg += String(start, ' ');
    msg += "^";
    msg += String(index_ - start - 1, '-');
    msg += " unknown identify";
    throw cfg::ident_error(String(this->str) + "\n" + msg);
  }
}

Option<Token::ident>
Lexer::to_ident(StringRef s) const noexcept {
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

Cfg::Ident
Cfg::from_token_ident(Token::ident ident) const {
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

bool
CfgExpr::match() const {
  switch (this->kind) {
    case Kind::cfg:
      return std::get<expr_type>(this->expr)->match();
    case Kind::not_:
      return !(std::get<expr_type>(this->expr)->match());
    case Kind::all: {
      bool res = true;
      for (const CfgExpr& c : std::get<expr_list_type>(this->expr)) {
        res &= c.match();
      }
      return res;
    }
    case Kind::any: {
      bool res = false;
      for (const CfgExpr& c : std::get<expr_list_type>(this->expr)) {
        res |= c.match();
      }
      return res;
    }
    case Kind::value:
      return this->match(std::get<Cfg>(this->expr));
    default:
      unreachable();
  }
}

bool
CfgExpr::match(const Cfg& c) const {
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
      unreachable();
  }
}

CfgExpr
Parser::expr() {
  if (const auto token = lexer.peek(); !token.has_value()) {
    throw cfg::expression_error("expected start of a cfg expression");
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

Cfg
Parser::cfg() {
  const usize index = lexer.index;
  if (const auto token = lexer.next(); !token.has_value()) {
    String msg = String(index + 1, ' ');
    msg += " ^ expected operator, but cfg expression ended";
    throw cfg::syntax_error(String(lexer.str) + "\n" + msg);
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
  throw cfg::syntax_error(String(lexer.str) + "\n" + msg);
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
  throw cfg::operator_error(String(lexer.str) + "\n" + msg);
}

Cfg
Parser::cfg_str(const usize index, Token::ident ident, Cfg::Op op) {
  const Cfg c = this->cfg_str(ident, op);
  if (ident != Token::ident::os_version) {
    this->throw_operator_error(index, op);
  }
  return c;
}

Cfg
Parser::cfg_str(Token::ident ident, Cfg::Op op) {
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
      throw cfg::syntax_error(String(lexer.str) + "\n" + msg);
    }
  } else {
    String msg = String(index, ' ');
    msg += "^ expected a string, but cfg expression ended";
    throw cfg::syntax_error(String(lexer.str) + "\n" + msg);
  }
}

bool
Parser::r_try(Token::Kind kind) {
  if (const auto token = lexer.peek()) {
    if (token->kind == kind) {
      this->lexer.next();
      return true;
    }
  }
  return false;
}

void
Parser::eat_left_paren(Token::ident prev) {
  const usize index = lexer.index;
  if (const auto token = lexer.next()) {
    if (token->kind != Token::LeftParen) {
      String msg = String(index, ' ');
      msg += "^ excepted '(' after `" + to_string(prev) + "`";
      throw cfg::syntax_error(String(lexer.str) + "\n" + msg);
    }
  } else {
    String msg = String(index, ' ');
    msg += "^ expected '(', but cfg expression ended";
    throw cfg::syntax_error(String(lexer.str) + "\n" + msg);
  }
}
void
Parser::eat_right_paren() {
  const usize index = lexer.index;
  if (const auto token = lexer.next()) {
    if (token->kind != Token::RightParen) {
      String msg = String(index, ' ');
      msg += "^";
      msg += String(lexer.index - index - 1, '-');
      msg += " excepted ')'";
      throw cfg::syntax_error(String(lexer.str) + "\n" + msg);
    }
  } else {
    String msg = String(index, ' ');
    msg += "^ expected ')', but cfg expression ended";
    throw cfg::syntax_error(String(lexer.str) + "\n" + msg);
  }
}

} // end namespace poac::util::cfg
