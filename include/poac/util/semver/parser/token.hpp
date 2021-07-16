#ifndef SEMVER_PARSER_TOKEN_HPP
#define SEMVER_PARSER_TOKEN_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <utility>

namespace semver::parser {
    struct Token {
        enum Kind {
            /// `=`
            Eq,
            /// `>`
            Gt,
            /// `<`
            Lt,
            /// `<=`
            LtEq,
            /// `>=`
            GtEq,
            /// '^`
            Caret,
            /// '~`
            Tilde,
            /// '*`
            Star,
            /// `.`
            Dot,
            /// `,`
            Comma,
            /// `-`
            Hyphen,
            /// `+`
            Plus,
            /// '||'
            Or,
            /// any number of whitespace (`\t\r\n `) and its span.
            Whitespace,
            /// Numeric component, like `0` or `42`.
            Numeric,
            /// Alphanumeric component, like `alpha1` or `79deadbe`.
            AlphaNumeric,
            /// UnexpectedChar
            Unexpected
        };

        using null_type = std::monostate;
        using whitespace_type = std::pair<std::size_t, std::size_t>;
        using numeric_type = std::uint_fast64_t;
        using alphanumeric_type = std::string_view;
        using variant_type =
            std::variant<null_type, whitespace_type, numeric_type, alphanumeric_type>;

        Kind kind;
        variant_type component;

        constexpr Token() noexcept
            : Token(Kind::Unexpected)
        {} // delegation

        constexpr explicit
        Token(Kind k) noexcept
            : kind(k), component()
        {}

        constexpr
        Token(Kind k, const std::size_t& s1, const std::size_t& s2)
            : kind(k != Kind::Whitespace
                       ? throw std::invalid_argument("semver::Token")
                       : Kind::Whitespace)
            , component(std::make_pair(s1, s2))
        {}

        constexpr
        Token(Kind k, const numeric_type& n)
            : kind(k != Kind::Numeric
                       ? throw std::invalid_argument("semver::Token")
                       : Kind::Numeric)
            , component(n)
        {}

        constexpr
        Token(Kind k, alphanumeric_type c)
            : kind(k != Kind::AlphaNumeric
                       ? throw std::invalid_argument("semver::Token")
                       : Kind::AlphaNumeric)
            , component(c)
        {}

        Token(const Token&) = default;
        Token& operator=(const Token&) = default;
        Token(Token&&) noexcept = default;
        Token& operator=(Token&&) noexcept = default;
        ~Token() = default;

        constexpr bool
        is_whitespace() const noexcept {
            return kind == Kind::Whitespace;
        }

        constexpr bool
        is_simple_token() const noexcept {
            return std::holds_alternative<null_type>(component);
        }

        constexpr bool
        is_wildcard() const noexcept {
            return kind == Kind::Star
                   || (std::holds_alternative<alphanumeric_type>(component)
                       && (std::get<alphanumeric_type>(component) == "X"
                           || std::get<alphanumeric_type>(component) == "x"
                       ));
        }
    };

    constexpr bool
    operator==(const Token& lhs, const Token& rhs) {
        if (lhs.is_simple_token() && rhs.is_simple_token()) {
            return lhs.kind == rhs.kind;
        }
        return (lhs.kind == rhs.kind)
            && (lhs.component == rhs.component);
    }
    constexpr bool
    operator==(const Token& lhs, const Token::Kind& rhs) {
        return lhs.is_simple_token() && (lhs.kind == rhs);
    }
    constexpr bool
    operator==(const Token::Kind& lhs, const Token& rhs) {
        return rhs.is_simple_token() && (lhs == rhs.kind);
    }

    constexpr bool
    operator!=(const Token& lhs, const Token& rhs) {
        return !(lhs == rhs);
    }
    constexpr bool
    operator!=(const Token& lhs, const Token::Kind& rhs) {
        return !(lhs == rhs);
    }
    constexpr bool
    operator!=(const Token::Kind& lhs, const Token& rhs) {
        return !(lhs == rhs);
    }

    struct Identifier {
        enum Kind {
            /// An identifier that's solely numbers.
            Numeric,
            /// An identifier with letters and numbers.
            AlphaNumeric
        };

        using numeric_type = std::uint_fast64_t;
        using alphanumeric_type = std::string_view;
        using variant_type = std::variant<numeric_type, alphanumeric_type>;

        Kind kind;
        variant_type component;

        Identifier() = delete;
        Identifier(const Identifier&) = default;
        Identifier& operator=(const Identifier&) = default;
        Identifier(Identifier&&) noexcept = default;
        Identifier& operator=(Identifier&&) noexcept = default;
        ~Identifier() = default;

        constexpr
        Identifier(Kind k, const numeric_type& n)
            : kind(k != Kind::Numeric
                      ? throw std::invalid_argument("semver::Identifier")
                      : Kind::Numeric)
            , component(n)
        {}

        constexpr
        Identifier(Kind k, alphanumeric_type c)
            : kind(k != Kind::AlphaNumeric
                      ? throw std::invalid_argument("semver::Identifier")
                      : Kind::AlphaNumeric)
            , component(c)
        {}
    };

    constexpr bool
    operator==(const Identifier& lhs, const Identifier& rhs) {
        return (lhs.kind == rhs.kind)
            && (lhs.component == rhs.component);
    }
    constexpr bool
    operator!=(const Identifier& lhs, const Identifier& rhs) {
        return !(lhs == rhs);
    }

    struct Version {
        /// Major version as number (`0` in `"0.1.2"`).
        std::uint_fast64_t major;
        /// Minor version as number (`1` in `"0.1.2"`).
        std::uint_fast64_t minor;
        /// Patch version as number (`2` in `"0.1.2"`).
        std::uint_fast64_t patch;
        /// Pre-release metadata as a vector of `Identifier` (`"alpha1"` in `"0.1.2-alpha1"`
        /// or `7` (numeric) in `"0.1.2-7"`, `"pre"` and `0` (numeric) in `"0.1.2-pre.0"`).
        std::vector<Identifier> pre;
        /// Build metadata as a vector of `Identifier` (`"build1"` in `"0.1.2+build1"`
        /// or `7` (numeric) in `"0.1.2+7"`, `"build"` and `0` (numeric) in `"0.1.2+pre.0"`).
        std::vector<Identifier> build;
    };
} // end namespace semver::parser

#endif // !SEMVER_PARSER_TOKEN_HPP
