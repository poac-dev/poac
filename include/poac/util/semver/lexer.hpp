#ifndef SEMVER_LEXER_HPP
#define SEMVER_LEXER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <utility>

namespace semver {
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

    struct Token {
        using whitespace_type = std::tuple<std::size_t, std::size_t>;
        using numeric_type = std::uint64_t;
        using alphanumeric_type = std::string_view;

        Kind kind;
        std::variant<std::monostate, whitespace_type, numeric_type, alphanumeric_type> component;

        constexpr explicit
        Token(Kind k) noexcept
            : kind(k), component()
        {}

        constexpr
        Token(Kind k, const std::size_t& s1, const std::size_t& s2)
            : kind(
                  k != Kind::Whitespace
                      ? throw std::invalid_argument("")
                      : Kind::Whitespace
              ),
              component(std::make_tuple(s1, s2))
        {}

        constexpr
        Token(Kind k, const numeric_type& n)
            : kind(
                  k != Kind::Numeric
                      ? throw std::invalid_argument("")
                      : Kind::Numeric
              ),
              component(n)
        {}

        constexpr
        Token(Kind k, alphanumeric_type c)
            : kind(
                  k != Kind::AlphaNumeric
                      ? throw std::invalid_argument("")
                      : Kind::AlphaNumeric
              ),
              component(c)
        {}

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
        return ('A' <= c && c < 'Z')
            || ('a' <= c && c < 'z');
    }

    constexpr bool
    is_alpha_numeric(const char& c) noexcept {
        return is_digit(c) || is_alphabet(c);
    }

    class Lexer {
    public:
        using string_type = std::string_view;
        using value_type = string_type::value_type;
        using size_type = std::size_t;

        size_type c1_index;
        value_type c1;
        value_type c2;
        string_type chars;


        explicit
        Lexer(string_type s)
            : c1_index(0)
            , c1(s[c1_index])
            , c2(s[c1_index + 1])
            , chars(s)
        {}

        Token
        next() {
            // two subsequent char tokens.
            const auto [a, b] = two();
            if (a == '<' && b == '=') {
                step_n(2);
                return Token{ Kind::LtEq };
            } else if (a == '>' && b == '=') {
                step_n(2);
                return Token{ Kind::GtEq };
            } else if (a == '|' && b == '|') {
                step_n(2);
                return Token{ Kind::Or };
            }

            // single char and start of numeric tokens.
            const value_type c = one();
            if (is_whitespace(c)) {
                return whitespace();
            } else if (c == '=') {
                step();
                return Token{ Kind::Eq };
            } else if (c == '>') {
                step();
                return Token{ Kind::Gt };
            } else if (c == '<') {
                step();
                return Token{ Kind::Lt };
            } else if (c == '^') {
                step();
                return Token{ Kind::Caret };
            } else if (c == '~') {
                step();
                return Token{ Kind::Tilde };
            } else if (c == '*') {
                step();
                return Token{ Kind::Star };
            } else if (c == '.') {
                step();
                return Token{ Kind::Dot };
            } else if (c == ',') {
                step();
                return Token{ Kind::Comma };
            } else if (c == '-') {
                step();
                return Token{ Kind::Hyphen };
            } else if (c == '+') {
                step();
                return Token{ Kind::Plus };
            } else if (is_alpha_numeric(c1)) {
                return component();
            }
            return Token{ Kind::Unexpected };
        }

        std::vector<Token>
        to_vec() {
            std::vector<Token> tokens;
            for (auto token = next(); token.kind != Kind::Unexpected; token = next()) {
                tokens.emplace_back(token);
            }
            return tokens;
        }

    private:
        /// Access the one character, or set it if it is not set.
        value_type one() noexcept {
            return c1;
        }

        /// Access two characters.
        std::pair<value_type, value_type>
        two() noexcept {
            return { c1, c2 };
        }

        /// Shift all lookahead storage by one.
        void step() noexcept {
            c1 = c2;
            c2 = chars[(++c1_index) + 1];
        }

        void step_n(const size_type& n) noexcept {
            for (size_type i = 0; i < n; ++i) {
                step();
            }
        }

        /// Consume a component.
        ///
        /// A component can either be an alphanumeric or numeric.
        /// Does not permit leading zeroes if numeric.
        Token component() {
            if (is_alphabet(one())) {
                const size_type start = c1_index;
                while (is_alpha_numeric(one())) {
                    step();
                }
                return Token{ Kind::AlphaNumeric, chars.substr(start, c1_index - start) };
            }

            const char* start = chars.begin() + c1_index;

            // exactly zero
            if (*start == '0' && !is_digit(*(start + 1))) {
                return Token{ Kind::Numeric, 0 };
            }

            const size_type start_index = c1_index;
            while (is_digit(one())) {
                step();
            }

            if (*start != '0' && !is_alphabet(one())) {
                return Token{ Kind::Numeric, std::stoi(std::string(start, c1_index)) };
            }

            while (is_alphabet(one())) {
                step();
            }
            return Token{ Kind::AlphaNumeric, chars.substr(start_index, c1_index - start_index) };
        }

        /// Consume whitespace.
        Token whitespace() noexcept {
            const size_type start = c1_index;

            while (is_whitespace(one())) {
                step();
            }
            return Token{ Kind::Whitespace, start, c1_index };
        }
    };
} // end namespace semver

#endif // !SEMVER_LEXER_HPP
