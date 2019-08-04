#ifndef POAC_UTIL_CFG_HPP
#define POAC_UTIL_CFG_HPP

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string_view>
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

    enum class compiler {
        gcc,
        clang,
        apple_clang,
        msvc,
        icc,
    };

//    enum class arch {
//        alpha,
//        arm,
//        blackfin,
//        convex,
//        ia64,
//        m68k,
//        mips,
//        s390x,
//        x86,
//        x86_32,
//        x86_64,
//    };

//    enum class feature {
//        aes,
//        avx,
//        avx2,
//        bmi1,
//        bmi2,
//        fma,
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
//    };

//    enum class os {
//        windows,
//        macos,
//        ios,
//        linux,
//        android,
//        freebsd,
//        dragonfly,
//        bitrig,
//        openbsd,
//        netbsd,
//    };
//
//    enum class family {
//        windows,
//        unix,
//    };

//    enum class platform {
//        mingw,
//    };

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

        Kind kind;
        std::string component;

        explicit
        Token(Kind k)
            : kind(k != Kind::String && k != Kind::Ident ? k
                   : throw std::invalid_argument("semver::Token"))
            , component()
        {}

        Token(Kind k, const std::string& s)
            : kind(k == Kind::String || k == Kind::Ident ? k
                   : throw std::invalid_argument("semver::Token"))
            , component(s)
        {}

        Token() = delete;
        Token(const Token&) = default;
        Token& operator=(const Token&) = default;
        Token(Token&&) noexcept = default;
        Token& operator=(Token&&) noexcept = default;
        ~Token() = default;

        friend std::ostream& operator<<(std::ostream& os, const Token& token);
    };

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
                os << "gt: >=";
                break;
            case Token::Lt:
                os << "lt: <";
                break;
            case Token::LtEq:
                os << "lt: <=";
                break;
            case Token::String:
                os << "string: ";
                os << token.component;
                break;
            case Token::Ident:
                os << "ident: ";
                os << token.component;
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
                    if (one() == '=') {
                        this->step();
                        return Token{ Token::GtEq };
                    } else {
                        return Token{ Token::Gt };
                    }
                case '<':
                    this->step();
                    if (one() == '=') {
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

    private:
        void step() noexcept {
            if (this->index < this->str.size()) {
                ++this->index;
            }
        }

        value_type one() const noexcept {
            return this->str[this->index];
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
            return Token{ Token::Ident, s };
        }
    };

    struct Parser {
        // std::vector<Token> tokens;

//        bool match() const {
//
//        }

        void eat() {

        }
    };

//    Parser parse(std::string_view s) {
//    }

    void parse(const std::string& s) {
        Lexer lex(s);
        for (auto token = lex.next(); token.has_value(); token = lex.next()) {
            std::cout << token.value() << std::endl;
        }
        std::cout << "end." << std::endl << std::endl;
    }

} // end namespace poac::util::cfg

#endif // !POAC_UTIL_CFG_HPP
