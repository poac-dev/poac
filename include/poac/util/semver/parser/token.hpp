#ifndef SEMVER_PARSER_TOKEN_HPP
#define SEMVER_PARSER_TOKEN_HPP

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
        using numeric_type = std::uint64_t;
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
        Token(Kind k, const alphanumeric_type& c)
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
        is_whildcard() const noexcept {
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


//    struct Identifier {
//        /// An identifier that's solely numbers.
//        Numeric(u64),
//        /// An identifier with letters and numbers.
//                AlphaNumeric(String),
//    };
} // end namespace semver::parser

#endif // !SEMVER_PARSER_TOKEN_HPP
