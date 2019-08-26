#ifndef SEMVER_PARSER_LEXER_HPP
#define SEMVER_PARSER_LEXER_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <variant>
#include <vector>
#include <optional>
#include <utility>

#include <poac/util/semver/parser/token.hpp>

namespace semver::parser {
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

    constexpr std::optional<std::uint_fast64_t>
    str_to_uint(std::string_view s) noexcept {
        std::uint_fast64_t i = 0;
        std::uint_fast64_t digit = 1;
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
        using size_type = std::size_t;
        using string_type = std::string_view;
        using value_type = string_type::value_type;
        using traits_type = string_type::traits_type;
        using const_iterator = string_type::const_iterator;
        using const_reverse_iterator = string_type::const_reverse_iterator;

        string_type str;
        size_type c1_index;

        explicit
        Lexer(string_type s)
            : str(s), c1_index(0)
        {}

        Token
        next() {
            // Check out of range
            if (c1_index > this->size()) {
                return Token{ Token::Unexpected };
            }

            // two subsequent char tokens.
            const auto [c1, c2] = this->two();
            if (c1 == '<' && c2 == '=') {
                this->step_n(2);
                return Token{ Token::LtEq };
            } else if (c1 == '>' && c2 == '=') {
                this->step_n(2);
                return Token{ Token::GtEq };
            } else if (c1 == '|' && c2 == '|') {
                this->step_n(2);
                return Token{ Token::Or };
            }

            // single char and start of numeric tokens.
            if (is_whitespace(c1)) {
                return whitespace();
            } else if (c1 == '=') {
                this->step();
                return Token{ Token::Eq };
            } else if (c1 == '>') {
                this->step();
                return Token{ Token::Gt };
            } else if (c1 == '<') {
                this->step();
                return Token{ Token::Lt };
            } else if (c1 == '^') {
                this->step();
                return Token{ Token::Caret };
            } else if (c1 == '~') {
                this->step();
                return Token{ Token::Tilde };
            } else if (c1 == '*') {
                this->step();
                return Token{ Token::Star };
            } else if (c1 == '.') {
                this->step();
                return Token{ Token::Dot };
            } else if (c1 == ',') {
                this->step();
                return Token{ Token::Comma };
            } else if (c1 == '-') {
                this->step();
                return Token{ Token::Hyphen };
            } else if (c1 == '+') {
                this->step();
                return Token{ Token::Plus };
            } else if (is_alpha_numeric(c1)) {
                return component();
            } else {
                this->step();
                return Token{ Token::Unexpected };
            }
        }

        constexpr size_type
        size() const noexcept {
            return str.size();
        }
        constexpr size_type
        max_size() const noexcept {
            return str.max_size();
        }
        constexpr bool
        empty() const noexcept {
            return str.empty();
        }

    private:
        void step() noexcept {
            ++c1_index;
        }

        void step_n(const size_type& n) noexcept {
            for (size_type i = 0; i < n; ++i) {
                step();
            }
        }

        /// Access the one character, or set it if it is not set.
        value_type
        one() const noexcept {
            return str[c1_index];
        }

        /// Access two characters.
        std::pair<value_type, value_type>
        two() const noexcept {
            return { str[c1_index], str[c1_index + 1] };
        }

        /// Consume a component.
        ///
        /// A component can either be an alphanumeric or numeric.
        /// Does not permit leading zeroes if numeric.
        Token
        component() {
            // e.g. abcde
            if (is_alphabet(this->one())) {
                const size_type start = this->c1_index;
                while (is_alpha_numeric(this->one())) {
                    this->step();
                }
                std::string_view sub = str.substr(start, this->c1_index - start);
                return Token{ Token::AlphaNumeric, sub };
            }

            // exactly zero
            if (const auto [c1, c2] = this->two(); c1 == '0' && !is_digit(c2)) {
                this->step();
                return Token{ Token::Numeric, 0 };
            }

            const size_type start = this->c1_index;
            while (is_digit(this->one())) {
                this->step();
            }
            if (str[start] != '0' && !is_alphabet(this->one())) {
                // e.g. 3425
                std::string_view sub = str.substr(start, this->c1_index - start);
                const std::uint_fast64_t value = str_to_uint(sub).value();
                return Token{ Token::Numeric, value };
            }

            // e.g. 3425dec
            while (is_alphabet(this->one())) {
                this->step();
            }
            std::string_view sub = str.substr(start, this->c1_index - start);
            return Token{ Token::AlphaNumeric, sub };
        }

        /// Consume whitespace.
        Token
        whitespace() noexcept {
            const size_type start = this->c1_index;
            while (is_whitespace(this->one())) {
                this->step();
            }
            return Token{ Token::Whitespace, start, this->c1_index };
        }
    };
} // end namespace semver::parser

#endif // !SEMVER_PARSER_LEXER_HPP
