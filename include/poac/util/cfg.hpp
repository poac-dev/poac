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
    public:
        virtual ~exception() noexcept override = default;
        virtual const char* what() const noexcept override { return ""; }
    };

    struct string_error : public cfg::exception {
    public:
        explicit string_error(const std::string& what_) : what_(what_) {}
        explicit string_error(const char* what_)        : what_(what_) {}
        virtual ~string_error() noexcept override = default;
        virtual const char* what() const noexcept override { return what_.c_str(); }

    protected:
        std::string what_;
    };

    struct ident_error : public cfg::exception {
    public:
        explicit ident_error(const std::string& what_) : what_(what_) {}
        explicit ident_error(const char* what_)        : what_(what_) {}
        virtual ~ident_error() noexcept override = default;
        virtual const char* what() const noexcept override { return what_.c_str(); }

    protected:
        std::string what_;
    };

    struct syntax_error : public cfg::exception {
    public:
        explicit syntax_error(const std::string& what_) : what_(what_) {}
        explicit syntax_error(const char* what_)        : what_(what_) {}
        virtual ~syntax_error() noexcept override = default;
        virtual const char* what() const noexcept override { return what_.c_str(); }

    protected:
        std::string what_;
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
            /// `<`
            Lt,
            /// `<=`
            LtEq,
            /// `>=`
            GtEq,
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

    std::ostream& operator<<(std::ostream& os, Token::ident ident) {
        os << to_string(ident);
        return os;
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
                os << token.id;
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
            : key(from_token_ident(key))
            , op(op != Op::Equals && this->key != Ident::os_version
                 ? throw cfg::syntax_error("You can not specify operators other than os_version except '='") : op)
            , value(value)
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
//        avx,
//        avx2,
//        bmi1,
//        bmi2,
#if BOOST_HW_SIMD_X86_FMA3_VERSION
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
//        sse4_1,
//        sse4_2,
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
                throw cfg::syntax_error("expected start of a cfg expression, found nothing");
            } else if (token->kind == Token::Ident) {
                if (token->id == Token::ident::all || token->id == Token::ident::any) {
                    this->lexer.next();
                    this->eat_left_paren(token->id);

                    std::vector<CfgExpr> e;
                    while (!r_try(Token::RightParen)) {
                        e.emplace_back(this->expr());
                        if (!this->r_try(Token::Comma)) {
                            this->eat_right_paren();
                            break;
                        }
                    }
                    if (token->id == Token::ident::all) {
                        return CfgExpr{ CfgExpr::all, e };
                    } else {
                        return CfgExpr{ CfgExpr::any, e };
                    }
                } else if (token->id == Token::ident::not_ || token->id == Token::ident::cfg) {
                    this->lexer.next();
                    this->eat_left_paren(token->id);
                    CfgExpr&& e = this->expr();
                    this->eat_right_paren();
                    if (token->id == Token::ident::not_) {
                        return CfgExpr{ CfgExpr::not_, std::make_shared<CfgExpr>(std::move(e)) };
                    } else {
                        return CfgExpr{ CfgExpr::cfg, std::make_shared<CfgExpr>(std::move(e)) };
                    }

                }
            }
            return CfgExpr{ CfgExpr::value, this->cfg() };
        }

        Cfg cfg() {
            if (const auto token = lexer.next(); !token.has_value()) {
                throw cfg::syntax_error("expected identifier, found nothing");
            } else if (token->kind == Token::Ident) {
                if (this->r_try(Token::Equals)) {
                    return this->cfg_str(token->id, Cfg::Op::Equals);
                } else if (this->r_try(Token::Gt)) {
                    return this->cfg_str(token->id, Cfg::Op::Gt);
                } else if (this->r_try(Token::GtEq)) {
                    return this->cfg_str(token->id, Cfg::Op::GtEq);
                } else if (this->r_try(Token::Lt)) {
                    return this->cfg_str(token->id, Cfg::Op::Lt);
                } else if (this->r_try(Token::LtEq)) {
                    return this->cfg_str(token->id, Cfg::Op::LtEq);
                }
            }
            throw cfg::syntax_error("expected identifier, found {}");
        }

    private:
        Cfg cfg_str(Token::ident ident, Cfg::Op op) {
            if (const auto t = lexer.next()) {
                if (t->kind == Token::String) {
                    return { ident, op, t->str };
                } else {
                    throw cfg::syntax_error("expected a string, found {}");
                }
            } else {
                throw cfg::syntax_error("expected a string, found nothing");
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
            if (const auto token = lexer.next()) {
                if (token->kind != Token::LeftParen) {
                    throw cfg::syntax_error("excepted '(' after `" + to_string(prev) + "`");
                }
            } else {
                throw cfg::syntax_error("expected '(', but cfg expr ended");
            }
        }
        void eat_right_paren() {
            if (const auto token = lexer.next()) {
                if (token->kind != Token::RightParen) {
                    throw cfg::syntax_error("excepted ')' to match this '('");
                }
            } else {
                throw cfg::syntax_error("expected ')', but cfg expr ended");
            }
        }
    };

    CfgExpr parse(const std::string& s) {
         return Parser(s).expr();
    }
} // end namespace poac::util::cfg

#endif // !POAC_UTIL_CFG_HPP
