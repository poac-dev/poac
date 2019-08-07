#ifndef POAC_UTIL_CFG_HPP
#define POAC_UTIL_CFG_HPP

#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include <boost/predef.h>

namespace poac::util::cfg {
    struct exception : public std::exception {
        explicit exception(const std::string& what_) : what_(what_) {}
        explicit exception(const char* what_)        : what_(what_) {}
        virtual ~exception() noexcept override = default;
        virtual const char* what() const noexcept override { return what_.c_str(); }

        exception(const exception&) = default;
        exception& operator=(const exception&) = default;
        exception(exception&&) noexcept = default;
        exception& operator=(exception&&) noexcept = default;

    private:
        std::string what_;
    };

    struct string_error : public cfg::exception {
        explicit string_error(const std::string& what_) : exception(what_) {}
        explicit string_error(const char* what_)        : exception(what_) {}
        virtual ~string_error() noexcept override = default;

        string_error(const string_error&) = default;
        string_error& operator=(const string_error&) = default;
        string_error(string_error&&) noexcept = default;
        string_error& operator=(string_error&&) noexcept = default;
    };

    struct ident_error : public cfg::exception {
    public:
        explicit ident_error(const std::string& what_) : exception(what_) {}
        explicit ident_error(const char* what_)        : exception(what_) {}
        virtual ~ident_error() noexcept override = default;

        ident_error(const ident_error&) = default;
        ident_error& operator=(const ident_error&) = default;
        ident_error(ident_error&&) noexcept = default;
        ident_error& operator=(ident_error&&) noexcept = default;
    };

    struct operator_error : public cfg::exception {
    public:
        explicit operator_error(const std::string& what_) : exception(what_) {}
        explicit operator_error(const char* what_)        : exception(what_) {}
        virtual ~operator_error() noexcept override = default;

        operator_error(const operator_error&) = default;
        operator_error& operator=(const operator_error&) = default;
        operator_error(operator_error&&) noexcept = default;
        operator_error& operator=(operator_error&&) noexcept = default;
    };

    struct expr_error : public cfg::exception {
    public:
        explicit expr_error(const std::string& what_) : exception(what_) {}
        explicit expr_error(const char* what_)        : exception(what_) {}
        virtual ~expr_error() noexcept override = default;

        expr_error(const expr_error&) = default;
        expr_error& operator=(const expr_error&) = default;
        expr_error(expr_error&&) noexcept = default;
        expr_error& operator=(expr_error&&) noexcept = default;
    };

    struct syntax_error : public cfg::exception {
    public:
        explicit syntax_error(const std::string& what_) : exception(what_) {}
        explicit syntax_error(const char* what_)        : exception(what_) {}
        virtual ~syntax_error() noexcept override = default;

        syntax_error(const syntax_error&) = default;
        syntax_error& operator=(const syntax_error&) = default;
        syntax_error(syntax_error&&) noexcept = default;
        syntax_error& operator=(syntax_error&&) noexcept = default;
    };


    constexpr bool
    is_ident_start(const char& c) noexcept {
        return (c == '_') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
    }

    constexpr bool
    is_ident_rest(const char& c) noexcept {
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
        std::string str;
        ident id;

        explicit
        Token(Kind k)
            : kind(k != Kind::String && k != Kind::Ident ? k
                   : throw std::invalid_argument("poac::util::cfg::Token"))
        {}

        Token(Kind k, const std::string& s)
            : kind(k == Kind::String ? k
                   : throw std::invalid_argument("poac::util::cfg::Token"))
            , str(s)
        {}

        Token(Kind k, ident i)
            : kind(k == Kind::Ident ? k
                   : throw std::invalid_argument("poac::util::cfg::Token"))
            , id(i)
        {}

        Token() = delete;
        Token(const Token&) = default;
        Token& operator=(const Token&) = default;
        Token(Token&&) noexcept = default;
        Token& operator=(Token&&) noexcept = default;
        ~Token() = default;

        friend std::ostream& operator<<(std::ostream& os, const Token& token);
    };

    std::string to_string(Token::ident ident) noexcept {
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
        }
    }

    std::ostream& operator<<(std::ostream& os, const Token& token) {
        switch (token.kind) {
            case Token::LeftParen:
                os << "left_paren: (";
                break;
            case Token::RightParen:
                os << "right_paren: )";
                break;
            case Token::Comma:
                os << "comma: ,";
                break;
            case Token::Equals:
                os << "equals: =";
                break;
            case Token::Gt:
                os << "gt: >";
                break;
            case Token::GtEq:
                os << "gteq: >=";
                break;
            case Token::Lt:
                os << "lt: <";
                break;
            case Token::LtEq:
                os << "lteq: <=";
                break;
            case Token::String:
                os << "string: ";
                os << token.str;
                break;
            case Token::Ident:
                os << "ident: ";
                os << to_string(token.id);
                break;
        }
        return os;
    }

    struct Lexer {
        using size_type = std::size_t;
        using string_type = std::string;
        using value_type = string_type::value_type;

        string_type str;
        size_type index;

        explicit Lexer(const string_type& str)
            : str(str), index(0)
        {}

        std::optional<Token>
        next() {
            if (this->index >= this->str.size()) {
                return std::nullopt;
            }
            switch (this->one()) {
                case ' ':
                    do {
                        this->step();
                    } while (this->one() == ' ');
                    return next();
                case '(':
                    this->step();
                    return Token{ Token::LeftParen };
                case ')':
                    this->step();
                    return Token{ Token::RightParen };
                case ',':
                    this->step();
                    return Token{ Token::Comma };
                case '=':
                    this->step();
                    return Token{ Token::Equals };
                case '>':
                    this->step();
                    if (this->one() == '=') {
                        this->step();
                        return Token{ Token::GtEq };
                    } else {
                        return Token{ Token::Gt };
                    }
                case '<':
                    this->step();
                    if (this->one() == '=') {
                        this->step();
                        return Token{ Token::LtEq };
                    } else {
                        return Token{ Token::Lt };
                    }
                case '"':
                    return string();
                default:
                    return ident();
            }
        }

        std::optional<Token>
        peek() const {
            return this->peek(this->index);
        }

    private:
        std::optional<Token>
        peek(size_type index_copy) const {
            if (index_copy >= this->str.size()) {
                return std::nullopt;
            }
            switch (this->str[index_copy]) {
                case ' ': {
                    do {
                        ++index_copy;
                    } while (this->str[index_copy] == ' ');
                    return peek(index_copy);
                }
                case '(':
                    return Token{ Token::LeftParen };
                case ')':
                    return Token{ Token::RightParen };
                case ',':
                    return Token{ Token::Comma };
                case '=':
                    return Token{ Token::Equals };
                case '>':
                    if (this->str[index_copy + 1] == '=') {
                        return Token{ Token::GtEq };
                    } else {
                        return Token{ Token::Gt };
                    }
                case '<':
                    if (this->str[index_copy + 1] == '=') {
                        return Token{ Token::LtEq };
                    } else {
                        return Token{ Token::Lt };
                    }
                case '"':
                    return string_copy(index_copy);
                default:
                    return ident_copy(index_copy);
            }
        }

        void step() noexcept {
            if (this->index < this->str.size()) {
                ++this->index;
            }
        }

        value_type one() const noexcept {
            return this->str[this->index];
        }
        value_type one(const size_type index_) const noexcept {
            return this->str[index_];
        }

        std::optional<Token>
        string() {
            this->step();
            const size_type start = this->index;
            while (this->one() != '"') {
                this->step();
                if (this->index >= this->str.size()) {
                    std::string msg;
                    msg += std::string(start - 1, ' ');
                    msg += "^";
                    msg += std::string(this->str.size() - start, '-');
                    msg += " unterminated string";
                    throw cfg::string_error(str + "\n" + msg);
                }
            }
            const string_type s = str.substr(start, this->index - start);
            this->step();
            return Token{ Token::String, s };
        }

        std::optional<Token>
        string_copy(size_type index_copy) const {
            ++index_copy;
            const size_type start = index_copy;
            while (this->str[index_copy] != '"') {
                ++index_copy;
                if (index_copy >= this->str.size()) {
                    std::string msg;
                    msg += std::string(start - 1, ' ');
                    msg += "^";
                    msg += std::string(this->str.size() - start, '-');
                    msg += " unterminated string";
                    throw cfg::string_error(str + "\n" + msg);
                }
            }
            const string_type s = str.substr(start, index_copy - start);
            return Token{ Token::String, s };
        }

        std::optional<Token>
        ident() {
            if (!is_ident_start(this->one())) {
                std::string msg;
                msg += std::string(this->index, ' ');
                msg += "^ unexpected character";
                throw cfg::ident_error(str + "\n" + msg);
            }
            const size_type start = this->index;
            this->step();
            while (is_ident_rest(this->one())) {
                this->step();
            }

            const string_type s = str.substr(start, this->index - start);
            if (const auto ident = to_ident(s)) {
                return Token{ Token::Ident, ident.value() };
            } else {
                std::string msg;
                msg += std::string(start, ' ');
                msg += "^";
                msg += std::string(this->index - start - 1, '-');
                msg += " unknown identify";
                throw cfg::ident_error(str + "\n" + msg);
            }
        }

        std::optional<Token>
        ident_copy(size_type index_copy) const {
            if (!is_ident_start(this->str[index_copy])) {
                std::string msg;
                msg += std::string(index_copy, ' ');
                msg += "^ unexpected character";
                throw cfg::ident_error(str + "\n" + msg);
            }
            const size_type start = index_copy;
            ++index_copy;
            while (is_ident_rest(this->str[index_copy])) {
                ++index_copy;
            }

            const string_type s = str.substr(start, index_copy - start);
            if (const auto ident = to_ident(s)) {
                return Token{ Token::Ident, ident.value() };
            } else {
                std::string msg;
                msg += std::string(start, ' ');
                msg += "^";
                msg += std::string(index_copy - start - 1, '-');
                msg += " unknown identify";
                throw cfg::ident_error(str + "\n" + msg);
            }
        }

        std::optional<Token::ident>
        to_ident(const string_type& s) const noexcept {
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
                return std::nullopt;
            }
        }
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
        std::string value;

        Cfg(Token::ident key, Op op, const std::string& value)
            : key(from_token_ident(key)), op(op), value(value)
        {}

        Cfg() = delete;
        Cfg(const Cfg&) = default;
        Cfg& operator=(const Cfg&) = default;
        Cfg(Cfg&&) noexcept = default;
        Cfg& operator=(Cfg&&) noexcept = default;
        ~Cfg() = default;

    private:
        Ident from_token_ident(Token::ident ident) const {
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
        using variant_type = std::variant<null_type, std::shared_ptr<CfgExpr>, std::vector<CfgExpr>, Cfg>;

        Kind kind;
        variant_type expr;

        CfgExpr(Kind kind, std::shared_ptr<CfgExpr>&& expr)
            : kind(kind == Kind::not_ || kind == Kind::cfg ? kind
                   : throw std::invalid_argument("poac::util::cfg::CfgExpr"))
            , expr(std::move(expr))
        {}
        CfgExpr(Kind kind, const std::vector<CfgExpr>& expr)
            : kind(kind == Kind::all || kind == Kind::any ? kind
                   : throw std::invalid_argument("poac::util::cfg::CfgExpr"))
            , expr(expr)
        {}
        CfgExpr(Kind kind, const Cfg& c)
            : kind(kind == Kind::value ? kind
                   : throw std::invalid_argument("poac::util::cfg::CfgExpr"))
            , expr(c)
        {}

        CfgExpr() = delete;
        CfgExpr(const CfgExpr&) = default;
        CfgExpr& operator=(const CfgExpr&) = default;
        CfgExpr(CfgExpr&&) noexcept = default;
        CfgExpr& operator=(CfgExpr&&) noexcept = default;
        ~CfgExpr() = default;

        bool match() const {
            switch (this->kind) {
                case Kind::cfg:
                    return std::get<std::shared_ptr<CfgExpr>>(this->expr)->match();
                case Kind::not_:
                    return !(std::get<std::shared_ptr<CfgExpr>>(this->expr)->match());
                case Kind::all: {
                    bool res = true;
                    for (const auto& c : std::get<std::vector<CfgExpr>>(this->expr)) {
                        res &= c.match();
                    }
                    return res;
                }
                case Kind::any: {
                    bool res = false;
                    for (const auto& c : std::get<std::vector<CfgExpr>>(this->expr)) {
                        res |= c.match();
                    }
                    return res;
                }
                case Kind::value:
                    return this->match(std::get<Cfg>(this->expr));
            }
        }

    private:
        bool match(const Cfg& c) const {
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
#elif defined(__SSE__)
                    return c.value == "sse";
#elif defined(__SSE2__)
                    return c.value == "sse2";
#elif defined(__SSE3__)
                    return c.value == "sse3";
#elif defined(__SSE4_1__)
                    return c.value == "sse4.1";
#elif defined(__SSE4_2__)
                    return c.value == "sse4.2";
#elif defined(__SSSE3__)
                    return c.value == "ssse3";
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
#elif  BOOST_OS_BSD_NET
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
            }
        }
    };

    struct Parser {
        Lexer lexer;

        explicit Parser(const std::string& str)
            : lexer(str)
        {}

        CfgExpr expr() {
            if (const auto token = lexer.peek(); !token.has_value()) {
                throw cfg::expr_error("expected start of a cfg expression");
            } else if (token->kind == Token::Ident) {
                if (token->id == Token::ident::all
                 || token->id == Token::ident::any) {
                    this->lexer.next();
                    this->eat_left_paren(token->id);
                    std::vector<CfgExpr> e;
                    do { // `all` and `any` need at least one expression.
                        e.emplace_back(this->expr());
                        if (!this->r_try(Token::Comma)) {
                            this->eat_right_paren();
                            break;
                        }
                    } while (!r_try(Token::RightParen));
                    if (token->id == Token::ident::all) {
                        return CfgExpr{ CfgExpr::all, e };
                    } else {
                        return CfgExpr{ CfgExpr::any, e };
                    }
                } else if (token->id == Token::ident::not_
                        || token->id == Token::ident::cfg) {
                    this->lexer.next();
                    this->eat_left_paren(token->id);
                    CfgExpr&& e = this->expr();
                    this->eat_right_paren();
                    if (token->id == Token::ident::not_) {
                        return CfgExpr{
                            CfgExpr::not_,
                            std::make_shared<CfgExpr>(std::move(e))
                        };
                    } else {
                        return CfgExpr{
                            CfgExpr::cfg,
                            std::make_shared<CfgExpr>(std::move(e))
                        };
                    }

                }
            }
            return CfgExpr{ CfgExpr::value, this->cfg() };
        }

        Cfg cfg() {
            const std::size_t index = lexer.index;
            if (const auto token = lexer.next(); !token.has_value()) {
                std::string msg = std::string(index + 1, ' ');
                msg += "^ expected identifier";
                throw cfg::syntax_error(lexer.str + "\n" + msg);
            } else if (token->kind == Token::Ident) {
                if (this->r_try(Token::Equals)) {
                    return this->cfg_str(token->id, Cfg::Op::Equals);
                } else if (this->r_try(Token::Gt)) {
                    return this->cfg_str(lexer.index, token->id, Cfg::Op::Gt);
                } else if (this->r_try(Token::GtEq)) {
                    return this->cfg_str(lexer.index, token->id, Cfg::Op::GtEq);
                } else if (this->r_try(Token::Lt)) {
                    return this->cfg_str(lexer.index, token->id, Cfg::Op::Lt);
                } else if (this->r_try(Token::LtEq)) {
                    return this->cfg_str(lexer.index, token->id, Cfg::Op::LtEq);
                }
            }
            std::string msg = std::string(index + 1, ' ');
            msg += "^ expected identifier";
            throw cfg::syntax_error(lexer.str + "\n" + msg);
        }

    private:
        [[noreturn]] inline void
        throw_operator_error(const std::size_t index, Cfg::Op op) const {
            std::string msg;
            if (op == Cfg::Op::Gt || op == Cfg::Op::Lt) {
                msg += std::string(index - 1, ' ');
                msg += "^";
            } else if (op == Cfg::Op::GtEq || op == Cfg::Op::LtEq) {
                msg += std::string(index - 2, ' ');
                msg += "^-";
            }
            msg += " cannot be specified except os_version";
            throw cfg::operator_error(lexer.str + "\n" + msg);
        }

        Cfg cfg_str(const std::size_t index, Token::ident ident, Cfg::Op op) {
            const Cfg c = this->cfg_str(ident, op);
            if (ident != Token::ident::os_version) {
                this->throw_operator_error(index, op);
            }
            return c;
        }

        Cfg cfg_str(Token::ident ident, Cfg::Op op) {
            const std::size_t index = lexer.index;
            if (const auto t = lexer.next()) {
                if (t->kind == Token::String) {
                    return { ident, op, t->str };
                } else {
                    std::string msg = std::string(index + 1, ' ');
                    msg += "^";
                    msg += std::string(lexer.index - index - 2, '-');
                    msg += " expected a string";
                    throw cfg::syntax_error(lexer.str + "\n" + msg);
                }
            } else {
                std::string msg = std::string(index, ' ');
                msg += "^ expected a string, but cfg expression ended";
                throw cfg::syntax_error(lexer.str + "\n" + msg);
            }
        }

        bool r_try(Token::Kind kind) {
            if (const auto token = lexer.peek()) {
                if (token->kind == kind) {
                    this->lexer.next();
                    return true;
                }
            }
            return false;
        }

        void eat_left_paren(Token::ident prev) {
            const std::size_t index = lexer.index;
            if (const auto token = lexer.next()) {
                if (token->kind != Token::LeftParen) {
                    std::string msg = std::string(index, ' ');
                    msg += "^ excepted '(' after `" + to_string(prev) + "`";
                    throw cfg::syntax_error(lexer.str + "\n" + msg);
                }
            } else {
                std::string msg = std::string(index, ' ');
                msg += "^ expected '(', but cfg expression ended";
                throw cfg::syntax_error(lexer.str + "\n" + msg);
            }
        }
        void eat_right_paren() {
            const std::size_t index = lexer.index;
            if (const auto token = lexer.next()) {
                if (token->kind != Token::RightParen) {
                    std::string msg = std::string(index, ' ');
                    msg += "^";
                    msg += std::string(lexer.index - index - 1, '-');
                    msg += " excepted ')'";
                    throw cfg::syntax_error(lexer.str + "\n" + msg);
                }
            } else {
                std::string msg = std::string(index, ' ');
                msg += "^ expected ')', but cfg expression ended";
                throw cfg::syntax_error(lexer.str + "\n" + msg);
            }
        }
    };

    CfgExpr parse(const std::string& s) {
         return Parser(s).expr();
    }
} // end namespace poac::util::cfg

#endif // !POAC_UTIL_CFG_HPP
