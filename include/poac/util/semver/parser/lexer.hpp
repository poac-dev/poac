#ifndef SEMVER_PARSER_LEXER_HPP
#define SEMVER_PARSER_LEXER_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <variant>
#include <vector>
#include <optional>
#include <utility>

namespace semver::lexer {
    enum class Kind {
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

    class Token {
    public:
        using whitespace_type = std::pair<std::size_t, std::size_t>;
        using numeric_type = std::uint64_t;
        using alphanumeric_type = std::string_view;
        using variant_type =
                std::variant<std::monostate, whitespace_type,
                             numeric_type, alphanumeric_type>;

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
            : Token(k, Kind::Whitespace, std::make_pair(s1, s2))
        {} // delegation

        constexpr
        Token(Kind k, const numeric_type& n)
            : Token(k, Kind::Numeric, n)
        {} // delegation

        constexpr
        Token(Kind k, const alphanumeric_type& c)
            : Token(k, Kind::AlphaNumeric, c)
        {} // delegation

        Token(const Token&) = default;
        Token& operator=(const Token&) = default;
        Token(Token&&) noexcept = default;
        Token& operator=(Token&&) noexcept = default;

    private:
        constexpr
        Token(Kind k, Kind target, const variant_type& c)
            : kind(k != target
                      ? throw std::invalid_argument("semver::Token")
                      : target
              ),
              component(c)
        {}

    public:
        constexpr bool
        is_whitespace() const noexcept {
            return kind == Kind::Whitespace;
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
    is_whitespace(const char& c) noexcept {
        switch (c) {
            case ' ':
                [[fallthrough]];
            case '\t':
                [[fallthrough]];
            case '\r':
                [[fallthrough]];
            case '\n':
                return true;
            default:
                return false;
        }
    }

    constexpr bool
    is_digit(const char& c) noexcept {
        return '0' <= c && c <= '9';
    }

    constexpr bool
    is_alphabet(const char& c) noexcept {
        return ('A' <= c && c <= 'Z')
            || ('a' <= c && c <= 'z');
    }

    constexpr bool
    is_alpha_numeric(const char& c) noexcept {
        return is_digit(c) || is_alphabet(c);
    }

    constexpr std::optional<std::uint64_t>
    str_to_uint(std::string_view s) noexcept {
        std::uint64_t i = 0;
        std::uint64_t digit = 1;
        for (int size = s.size() - 1; size >= 0; --size) {
            char c = s[size];
            if (is_digit(c)) {
                i += (c - '0') * digit;
            } else {
                return std::nullopt;
            }
            digit *= 10;
        }
        return i;
    }

    class Lexer {
    public:
        using string_type = std::string_view;
        using value_type = string_type::value_type;
        using traits_type = string_type::traits_type;
        using size_type = std::size_t;

        string_type str;

        constexpr explicit
        Lexer(string_type s)
            : str(s)
        {}

//        template <typename... Args>
//        std::vector<Token>
//        to_std_vector(const size_type& i, Args&... tokens) const {
//            if (i < str.size()) {
//                const auto result = pick(i);
//                return to_std_vector(i + result.first, tokens..., result.second);
//            } else {
//                return { tokens... };
//            }
//        }
//        std::vector<Token>
//        to_std_vector() const {
//            const auto [count, token] = pick(0);
//            return to_std_vector(count, token);
//        }

        constexpr std::pair<std::size_t, Token>
        pick(const std::size_t& i) const {
            // two subsequent char tokens.
            const auto two_c = two(i);
            if (two_c.first == '<' && two_c.second == '=') {
                return { 2, Token{ Kind::LtEq } };
            } else if (two_c.first == '>' && two_c.second == '=') {
                return { 2, Token{ Kind::GtEq } };
            } else if (two_c.first == '|' && two_c.second == '|') {
                return { 2, Token{ Kind::Or } };
            }

            // single char and start of numeric tokens.
            const value_type c = one(i);
            if (is_whitespace(c)) {
                return whitespace(str, i);
            } else if (c == '=') {
                return { 1, Token{ Kind::Eq } };
            } else if (c == '>') {
                return { 1, Token{ Kind::Gt } };
            } else if (c == '<') {
                return { 1, Token{ Kind::Lt } };
            } else if (c == '^') {
                return { 1, Token{ Kind::Caret } };
            } else if (c == '~') {
                return { 1, Token{ Kind::Tilde } };
            } else if (c == '*') {
                return { 1, Token{ Kind::Star } };
            } else if (c == '.') {
                return { 1, Token{ Kind::Dot } };
            } else if (c == ',') {
                return { 1, Token{ Kind::Comma } };
            } else if (c == '-') {
                return { 1, Token{ Kind::Hyphen } };
            } else if (c == '+') {
                return { 1, Token{ Kind::Plus } };
            } else if (is_alpha_numeric(c)) {
                return component(str, i);
            }
            return { 1, Token{ Kind::Unexpected } };
        }

        constexpr size_type
        size() const noexcept {
            return str.size();
        }
        constexpr size_type
        max_size() const noexcept {
            return size();
        }
        constexpr bool
        empty() const noexcept {
            return size() == 0;
        }

    private:
        /// Access the one character, or set it if it is not set.
        constexpr value_type
        one(const size_type& i) const noexcept {
            return str[i];
        }

        /// Access two characters.
        constexpr std::pair<value_type, value_type>
        two(const size_type& i) const noexcept {
            return { str[i], str[i + 1] };
        }

        /// Consume a component.
        ///
        /// A component can either be an alphanumeric or numeric.
        /// Does not permit leading zeroes if numeric.
        constexpr std::pair<size_type, Token>
        component(std::string_view str, size_type i) const {
            // e.g. abcde
            if (is_alphabet(str[i])) {
                const size_type start = i;
                while (is_alpha_numeric(str[++i]));
                std::string_view sub = str.substr(start, i - start);
                return { i - start, Token{ Kind::AlphaNumeric, sub } };
            }

            // exactly zero
            if (str[i] == '0' && !is_digit(str[i + 1])) {
                return { 1, Token{ Kind::Numeric, 0 } };
            }

            const size_type start = i;
            while (is_digit(str[++i]));
            if (str[start] != '0' && !is_alphabet(str[i])) {
                // e.g. 3425
                std::string_view sub = str.substr(start, i - start);
                std::uint64_t value = str_to_uint(sub).value();
                return { i - start, Token{ Kind::Numeric, static_cast<std::size_t>(value) } };
            }

            // e.g. 3425dec
            while (is_alphabet(str[++i]));
            std::string_view sub = str.substr(start, i - start);
            return { i - start, Token{ Kind::AlphaNumeric, sub } };
        }

        /// Consume whitespace.
        constexpr std::pair<size_type, Token>
        whitespace(std::string_view str, size_type i) const noexcept {
            const size_type start = i;
            while (is_whitespace(str[++i]));
            return { i - start, Token{ Kind::Whitespace, start, i } };
        }
    };

//    template <std::size_t N>
//    constexpr std::array<Token, N>
//    lex(const char(&arr)[N]) {
//        return Lexer<N>(arr).to_array();
//    }
} // end namespace semver::lexer

#endif // !SEMVER_PARSER_LEXER_HPP
