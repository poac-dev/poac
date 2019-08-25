#ifndef POAC_UTIL_CFG_HPP
#define POAC_UTIL_CFG_HPP

#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <utility>

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

    protected:
        std::string what_;
    };

    struct string_error : public cfg::exception {
        explicit string_error(const std::string& what_)
            : exception("missing terminating '\"' character\n" + what_) {}
        explicit string_error(const char* what_) : string_error(std::string(what_)) {}
        virtual ~string_error() noexcept override = default;

        string_error(const string_error&) = default;
        string_error& operator=(const string_error&) = default;
        string_error(string_error&&) noexcept = default;
        string_error& operator=(string_error&&) noexcept = default;
    };

    struct ident_error : public cfg::exception {
    public:
        explicit ident_error(const std::string& what_)
            : exception("cfg expected parens, a comma, an identifier, or a string\n" + what_) {}
        explicit ident_error(const char* what_) : ident_error(std::string(what_)) {}
        virtual ~ident_error() noexcept override = default;

        ident_error(const ident_error&) = default;
        ident_error& operator=(const ident_error&) = default;
        ident_error(ident_error&&) noexcept = default;
        ident_error& operator=(ident_error&&) noexcept = default;
    };

    struct operator_error : public cfg::exception {
    public:
        explicit operator_error(const std::string& what_)
            : exception("cfg operator error\n" + what_) {}
        explicit operator_error(const char* what_) : operator_error(std::string(what_)) {}
        virtual ~operator_error() noexcept override = default;

        operator_error(const operator_error&) = default;
        operator_error& operator=(const operator_error&) = default;
        operator_error(operator_error&&) noexcept = default;
        operator_error& operator=(operator_error&&) noexcept = default;
    };

    struct expression_error : public cfg::exception {
    public:
        explicit expression_error(const std::string& what_) : exception(what_) {}
        explicit expression_error(const char* what_)        : exception(what_) {}
        virtual ~expression_error() noexcept override = default;

        expression_error(const expression_error&) = default;
        expression_error& operator=(const expression_error&) = default;
        expression_error(expression_error&&) noexcept = default;
        expression_error& operator=(expression_error&&) noexcept = default;
    };

    struct syntax_error : public cfg::exception {
    public:
        explicit syntax_error(const std::string& what_)
            : exception("cfg syntax error\n" + what_) {}
        explicit syntax_error(const char* what_) : syntax_error(std::string(what_)) {}
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
        using string_type = std::string_view;
        std::variant<std::monostate, string_type, ident> value;

        explicit
        Token(Kind k)
            : kind(k != Kind::String && k != Kind::Ident ? k
                   : throw std::invalid_argument("poac::util::cfg::Token"))
            , value()
        {}
        Token(Kind k, string_type s)
            : kind(k == Kind::String ? k
                   : throw std::invalid_argument("poac::util::cfg::Token"))
            , value(s)
        {}
        Token(Kind k, ident i)
            : kind(k == Kind::Ident ? k
                   : throw std::invalid_argument("poac::util::cfg::Token"))
            , value(i)
        {}

        Token() = delete;
        Token(const Token&) = default;
        Token& operator=(const Token&) = default;
        Token(Token&&) noexcept = default;
        Token& operator=(Token&&) noexcept = default;
        ~Token() = default;

        string_type
        get_str() const {
            return std::get<string_type>(this->value);
        }
        ident
        get_ident() const {
            return std::get<ident>(this->value);
        }

        friend std::ostream&
        operator<<(std::ostream& os, const Token& token);
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
            default:
                throw std::logic_error(
                        "To access out of range of the "
                        "enumeration values is undefined behavior.");
        }
    }

    std::ostream& operator<<(std::ostream& os, const Token& token) {
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
                throw std::logic_error(
                        "To access out of range of the "
                        "enumeration values is undefined behavior.");
        }
    }

    struct Lexer {
        using value_type = std::string_view::value_type;
        using size_type = std::size_t;

        std::string_view str;
        size_type index;

        explicit Lexer(std::string_view str)
            : str(str), index(0)
        {}

        std::optional<Token>
        next() {
            const auto [diff, token] = tokenize(this->index);
            this->step_n(diff);
            return token;
        }

        std::optional<Token>
        peek() const {
            const auto [diff, token] = tokenize(this->index);
            static_cast<void>(diff);
            return token;
        }

    private:
        std::pair<size_type, std::optional<Token>>
        tokenize(size_type index_) const {
            if (index_ >= this->str.size()) {
                return { this->diff_step(index_), std::nullopt };
            }
            switch (this->one(index_)) {
                case ' ':
                    do {
                        this->step(index_);
                    } while (this->one(index_) == ' ');
                    return tokenize(index_);
                case '(':
                    this->step(index_);
                    return { this->diff_step(index_), Token{Token::LeftParen} };
                case ')':
                    this->step(index_);
                    return { this->diff_step(index_), Token{Token::RightParen} };
                case ',':
                    this->step(index_);
                    return { this->diff_step(index_), Token{Token::Comma} };
                case '=':
                    this->step(index_);
                    return { this->diff_step(index_), Token{Token::Equals} };
                case '>':
                    this->step(index_);
                    if (this->one(index_) == '=') {
                        this->step(index_);
                        return { this->diff_step(index_), Token{Token::GtEq} };
                    } else {
                        return { this->diff_step(index_), Token{Token::Gt} };
                    }
                case '<':
                    this->step(index_);
                    if (this->one(index_) == '=') {
                        this->step(index_);
                        return { this->diff_step(index_), Token{Token::LtEq} };
                    } else {
                        return { this->diff_step(index_), Token{Token::Lt} };
                    }
                case '"':
                    return string(index_);
                default:
                    return ident(index_);
            }
        }

        void
        step(size_type& index_) const noexcept {
            if (index_ < this->str.size()) {
                ++index_;
            }
        }
        void
        step_n(const size_type n) noexcept {
            for (size_type i = 0; i < n; ++i) {
                this->step(this->index);
            }
        }

        size_type
        diff_step(const size_type index_) const noexcept {
            return index_ - this->index;
        }

        value_type
        one(const size_type index_) const noexcept {
            return this->str[index_];
        }

        std::pair<size_type, Token>
        string(size_type index_) const {
            this->step(index_);
            const size_type start = index_;
            while (this->one(index_) != '"') {
                this->step(index_);
                if (index_ >= this->str.size()) {
                    std::string msg;
                    msg += std::string(start - 1, ' ');
                    msg += "^";
                    msg += std::string(this->str.size() - start, '-');
                    msg += " unterminated string";
                    throw cfg::string_error(std::string(this->str) + "\n" + msg);
                }
            }
            std::string_view s = this->str.substr(start, index_ - start);
            this->step(index_);
            return { this->diff_step(index_), Token{Token::String, s} };
        }

        std::pair<size_type, Token>
        ident(size_type index_) const {
            if (!is_ident_start(this->one(index_))) {
                std::string msg;
                msg += std::string(index_, ' ');
                msg += "^ unexpected character";
                throw cfg::ident_error(std::string(this->str) + "\n" + msg);
            }
            const size_type start = index_;
            this->step(index_);
            while (is_ident_rest(this->one(index_))) {
                this->step(index_);
            }

            std::string_view s = this->str.substr(start, index_ - start);
            if (const auto ident = to_ident(s)) {
                return {
                    this->diff_step(index_),
                    Token{Token::Ident, ident.value()}
                };
            } else {
                std::string msg;
                msg += std::string(start, ' ');
                msg += "^";
                msg += std::string(index_ - start - 1, '-');
                msg += " unknown identify";
                throw cfg::ident_error(std::string(this->str) + "\n" + msg);
            }
        }

        std::optional<Token::ident>
        to_ident(std::string_view s) const noexcept {
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
        std::string_view value;

        Cfg(Token::ident key, Op op, std::string_view value)
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
        using expr_type = std::unique_ptr<CfgExpr>;
        using expr_list_type = std::vector<CfgExpr>;
        using variant_type = std::variant<null_type, expr_type, expr_list_type, Cfg>;

        Kind kind;
        variant_type expr;

        CfgExpr(Kind kind, expr_type&& expr)
            : kind(kind == Kind::not_ || kind == Kind::cfg ? kind
                   : throw std::invalid_argument("poac::util::cfg::CfgExpr"))
            , expr(std::move(expr))
        {}
        CfgExpr(Kind kind, expr_list_type&& expr)
            : kind(kind == Kind::all || kind == Kind::any ? kind
                   : throw std::invalid_argument("poac::util::cfg::CfgExpr"))
            , expr(std::move(expr))
        {}
        CfgExpr(Kind kind, const Cfg& c)
            : kind(kind == Kind::value ? kind
                   : throw std::invalid_argument("poac::util::cfg::CfgExpr"))
            , expr(c)
        {}

        CfgExpr() = delete;
        CfgExpr(const CfgExpr&) = delete;
        CfgExpr& operator=(const CfgExpr&) = delete;
        CfgExpr(CfgExpr&&) noexcept = default;
        CfgExpr& operator=(CfgExpr&&) noexcept = default;
        ~CfgExpr() = default;

        bool match() const {
            switch (this->kind) {
                case Kind::cfg:
                    return std::get<expr_type>(this->expr)->match();
                case Kind::not_:
                    return !(std::get<expr_type>(this->expr)->match());
                case Kind::all: {
                    bool res = true;
                    for (const auto& c : std::get<expr_list_type>(this->expr)) {
                        res &= c.match();
                    }
                    return res;
                }
                case Kind::any: {
                    bool res = false;
                    for (const auto& c : std::get<expr_list_type>(this->expr)) {
                        res |= c.match();
                    }
                    return res;
                }
                case Kind::value:
                    return this->match(std::get<Cfg>(this->expr));
                default:
                    throw std::logic_error(
                            "To access out of range of the "
                            "enumeration values is undefined behavior.");
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
                default:
                    throw std::logic_error(
                            "To access out of range of the "
                            "enumeration values is undefined behavior.");
            }
        }
    };

    struct Parser {
        Lexer lexer;

        explicit Parser(std::string_view str)
            : lexer(str)
        {}

        CfgExpr expr() {
            if (const auto token = lexer.peek(); !token.has_value()) {
                throw cfg::expression_error("expected start of a cfg expression");
            } else if (token->kind == Token::Ident) {
                if (token->get_ident() == Token::ident::all
                 || token->get_ident() == Token::ident::any) {
                    this->lexer.next();
                    this->eat_left_paren(token->get_ident());
                    std::vector<CfgExpr> e;
                    do { // `all` and `any` need at least one expression.
                        e.emplace_back(this->expr());
                        if (!this->r_try(Token::Comma)) {
                            this->eat_right_paren();
                            break;
                        }
                    } while (!r_try(Token::RightParen));
                    if (token->get_ident() == Token::ident::all) {
                        return CfgExpr{ CfgExpr::all, std::move(e) };
                    } else {
                        return CfgExpr{ CfgExpr::any, std::move(e) };
                    }
                } else if (token->get_ident() == Token::ident::not_
                        || token->get_ident() == Token::ident::cfg) {
                    this->lexer.next();
                    this->eat_left_paren(token->get_ident());
                    CfgExpr&& e = this->expr();
                    this->eat_right_paren();
                    if (token->get_ident() == Token::ident::not_) {
                        return CfgExpr{
                            CfgExpr::not_,
                            std::make_unique<CfgExpr>(std::move(e))
                        };
                    } else {
                        return CfgExpr{
                            CfgExpr::cfg,
                            std::make_unique<CfgExpr>(std::move(e))
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
                msg += " ^ expected operator, but cfg expression ended";
                throw cfg::syntax_error(std::string(lexer.str) + "\n" + msg);
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
            std::string msg = std::string(lexer.index + 1, ' ');
            msg += "^ expected operator";
            throw cfg::syntax_error(std::string(lexer.str) + "\n" + msg);
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
            throw cfg::operator_error(std::string(lexer.str) + "\n" + msg);
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
                    return { ident, op, t->get_str() };
                } else {
                    std::string msg = std::string(index + 1, ' ');
                    msg += "^";
                    const int range = lexer.index - index - 2;
                    msg += std::string(range < 0 ? 0 : range, '-');
                    msg += " expected a string";
                    throw cfg::syntax_error(std::string(lexer.str) + "\n" + msg);
                }
            } else {
                std::string msg = std::string(index, ' ');
                msg += "^ expected a string, but cfg expression ended";
                throw cfg::syntax_error(std::string(lexer.str) + "\n" + msg);
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
                    throw cfg::syntax_error(std::string(lexer.str) + "\n" + msg);
                }
            } else {
                std::string msg = std::string(index, ' ');
                msg += "^ expected '(', but cfg expression ended";
                throw cfg::syntax_error(std::string(lexer.str) + "\n" + msg);
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
                    throw cfg::syntax_error(std::string(lexer.str) + "\n" + msg);
                }
            } else {
                std::string msg = std::string(index, ' ');
                msg += "^ expected ')', but cfg expression ended";
                throw cfg::syntax_error(std::string(lexer.str) + "\n" + msg);
            }
        }
    };

    CfgExpr parse(std::string_view s) {
         return Parser(s).expr();
    }
} // end namespace poac::util::cfg

#endif // !POAC_UTIL_CFG_HPP
