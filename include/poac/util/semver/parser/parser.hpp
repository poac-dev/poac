#ifndef SEMVER_PARSER_PARSER_HPP
#define SEMVER_PARSER_PARSER_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>
#include <optional>

#include <poac/util/semver/parser/lexer.hpp>
#include <poac/util/semver/parser/range.hpp>
#include <poac/util/semver/parser/token.hpp>

namespace semver::parser {
    struct Parser {
        using string_type = std::string_view;
        using value_type = string_type::value_type;

        Lexer lexer;
        Token c1;

        /// Construct a new parser for the given input.
        explicit
        Parser(string_type str)
            : lexer(str), c1(lexer.next())
        {}

        /// Pop one token.
        inline Token
        pop() {
            Token c1 = this->c1;
            this->c1 = lexer.next();
            return c1;
        }

        /// Peek one token.
        inline Token
        peek() const noexcept {
            return this->c1;
        }

        /// Skip whitespace if present.
        void skip_whitespace() {
            if (peek().is_whitespace()) {
                pop();
            }
        }

        /// Parse an optional comma separator, then if that is present a predicate.
        std::optional<Predicate>
        comma_predicate() {
            const bool has_comma = has_ws_separator(Token::Comma);

            if (const auto predicate = this->predicate()) {
                return predicate;
            } else if (has_comma) {
                return std::nullopt; // Err(EmptyPredicate)
            } else {
                return std::nullopt;
            }
        }

        /// Parse an optional or separator `||`, then if that is present a range.
        std::optional<VersionReq>
        or_range() {
            if (!this->has_ws_separator(Token::Or)) {
                return std::nullopt;
            }
            return this->range();
        }

        /// Parse a single component.
        ///
        /// Returns `None` if the component is a wildcard.
        std::optional<std::uint64_t>
        component() {
            if (const Token token = this->pop(); token.kind == Token::Numeric) {
                return std::get<Token::numeric_type>(token.component);
            } else if (token.is_whildcard()) {
                return std::nullopt;
            } else {
                return std::nullopt; // Err(UnexpectedToken(tok))
            }
        }

        /// Parse a single numeric.
        std::optional<std::uint64_t>
        numeric() {
            if (const Token token = this->pop(); token.kind == Token::Numeric) {
                return std::get<Token::numeric_type>(token.component);
            }
            return std::nullopt;
        }

        /// Optionally parse a dot, then a component.
        ///
        /// The second component of the tuple indicates if a wildcard has been encountered, and is
        /// always `false` if the first component is `Some`.
        ///
        /// If a dot is not encountered, `(None, false)` is returned.
        ///
        /// If a wildcard is encountered, `(None, true)` is returned.
        std::optional<std::uint64_t>
        dot_component() {
            if (this->peek() != Token::Dot) {
                return std::nullopt;
            }
            // pop the peeked dot.
            this->pop();
            return this->component();
        }

        /// Parse a dot, then a numeric.
        std::optional<std::uint64_t>
        dot_numeric() {
            if (pop() != Token::Dot) {
                return std::nullopt;
            }
            return numeric();
        }

        /// Parse an string identifier.
        ///
        /// Like, `foo`, or `bar`.
        std::optional<Identifier>
        identifier() {
            const Token& token = pop();
            if (token.kind == Token::AlphaNumeric) {
                return Identifier(Identifier::AlphaNumeric, std::get<Token::alphanumeric_type>(token.component));
            }
            else if (token.kind == Token::Numeric) {
                return Identifier(Identifier::Numeric, std::get<Token::numeric_type>(token.component));
            }
            return std::nullopt;
        }

        /// Parse all pre-release identifiers, separated by dots.
        ///
        /// Like, `abcdef.1234`.
        std::vector<Identifier>
        pre() {
            if (peek() != Token::Hyphen) {
                return {};
            }
            // pop the peeked hyphen.
            pop();
            return parts();
        }

        /// Parse a dot-separated set of identifiers.
        std::vector<Identifier>
        parts() {
            std::vector<Identifier> parts{};
            parts.push_back(identifier().value());

            while (peek() == Token::Dot) {
                // pop the peeked hyphen.
                pop();
                parts.push_back(identifier().value());
            }
            return parts;
        }

        /// Parse optional build metadata.
        ///
        /// Like, `` (empty), or `+abcdef`.
        std::vector<Identifier>
        plus_build_metadata() {
            if (peek() != Token::Plus) {
                return {};
            }
            // pop the peeked plus.
            pop();
            return parts();
        }

        /// Optionally parse a single operator.
        ///
        /// Like, `~`, or `^`.
        Op op() {
            Op op(Op::Compatible);

            switch (peek().kind) {
                case Token::Eq:
                    op.kind = Op::Ex;
                case Token::Gt:
                    op.kind = Op::Gt;
                case Token::GtEq:
                    op.kind = Op::GtEq;
                case Token::Lt:
                    op.kind = Op::Lt;
                case Token::LtEq:
                    op.kind = Op::LtEq;
                case Token::Tilde:
                    op.kind = Op::Tilde;
                case Token::Caret:
                    op.kind = Op::Compatible;
                // default op
                default:
                    op.kind = Op::Compatible;
            }
            // remove the matched token.
            pop();
            skip_whitespace();
            return op;
        }

        /// Parse a single predicate.
        ///
        /// Like, `^1`, or `>=2.0.0`.
        std::optional<Predicate>
        predicate() {
            if (is_eof()) {
                return std::nullopt;
            }

            Op op = this->op();

            std::uint64_t major;
            if (const auto m = this->component()) {
                major = m.value();
            } else {
                return std::nullopt;
            }

            const auto minor = this->dot_component();
            const auto patch = this->dot_component();
            const auto pre = this->pre();

            // TODO: avoid illegal combinations, like `1.*.0`.
            if (!minor.has_value()) {
                op = Op(Op::Wildcard, WildcardVersion::Minor);
            }
            if (!patch.has_value()) {
                op = Op(Op::Wildcard, WildcardVersion::Patch);
            }

            // ignore build metadata
            this->plus_build_metadata();

            return Predicate{ op, major, minor, patch, pre };
        }

        /// Parse a single range.
        ///
        /// Like, `^1.0` or `>=3.0.0, <4.0.0`.
        VersionReq
        range() {
            std::vector<Predicate> predicates{};

            if (const auto predicate = this->predicate()) {
                predicates.push_back(predicate.value());

                while (const auto next = this->comma_predicate()) {
                    predicates.push_back(next.value());
                }
            }
            return VersionReq{ predicates };
        }

        /// Parse a comparator.
        ///
        /// Like, `1.0 || 2.0` or `^1 || >=3.0.0, <4.0.0`.
        Comparator
        comparator() {
            std::vector<VersionReq> ranges{};
            ranges.push_back(this->range());

            while (const auto next = this->or_range()) {
                ranges.push_back(next.value());
            }
            return Comparator{ ranges };
        }

        /// Parse a version.
        ///
        /// Like, `1.0.0` or `3.0.0-beta.1`.
        Version
        version() {
            this->skip_whitespace();

            const std::uint64_t major = this->numeric().value();
            const std::uint64_t minor = this->dot_numeric().value();
            const std::uint64_t patch = this->dot_numeric().value();
            const std::vector<Identifier> pre = this->pre();
            const std::vector<Identifier> build = this->plus_build_metadata();

            this->skip_whitespace();

            return Version{ major, minor, patch, pre, build };
        }

        /// Check if we have reached the end of input.
        bool is_eof() const {
            return lexer.max_size() < lexer.c1_index;
        }

        /// Get the rest of the tokens in the parser.
        ///
        /// Useful for debugging.
//        std::vector<Token>
//        tail() {
//            std::vector<Token> out{};
//
//            out.push_back(c1);
//            for (std::size_t i = c1_index; i < lexer.size(); ) {
//                const auto [count, token] = lexer.pick(i);
//                out.push_back(token);
//                i += count;
//            }
//            return out;
//        }

    private:
        bool has_ws_separator(const Token::Kind& pat) {
            skip_whitespace();

            if (peek() == pat) {
                // pop the separator.
                pop();
                // strip suffixing whitespace.
                skip_whitespace();
                return true;
            }
            return false;
        }
    };
} // end namespace semver::parser

#endif // !SEMVER_PARSER_PARSER_HPP
