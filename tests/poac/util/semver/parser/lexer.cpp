#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/util/semver/parser/lexer.hpp>

BOOST_AUTO_TEST_CASE( semver_lexer_simple_tokens_test )
{
    using semver::parser::Lexer;
    using semver::parser::Token;

    Lexer lexer{"=><<=>=^~*.,-+||"};
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Eq;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Gt;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Lt;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::LtEq;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::GtEq;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Caret;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Tilde;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Star;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Dot;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Comma;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Hyphen;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Plus;
        BOOST_CHECK(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Token::Or;
        BOOST_CHECK(test);
    }
}

BOOST_AUTO_TEST_CASE( semver_lexer_lexer_whitespace_test )
{
    using semver::parser::Lexer;
    using semver::parser::Token;

    Lexer lexer{"  foo \t\n\rbar"};
    {
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::Whitespace;
        BOOST_CHECK(test1);

        BOOST_CHECK(std::holds_alternative<Token::whitespace_type>(token.component));
        const bool test2 = std::get<Token::whitespace_type>(token.component) == Token::whitespace_type(0, 2);
        BOOST_CHECK(test2);
    }
    {
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::AlphaNumeric;
        BOOST_CHECK(test1);

        BOOST_CHECK(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "foo";
        BOOST_CHECK(test2);
    }
    {
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::Whitespace;
        BOOST_CHECK(test1);

        BOOST_CHECK(std::holds_alternative<Token::whitespace_type>(token.component));
        const bool test2 = std::get<Token::whitespace_type>(token.component) == Token::whitespace_type(5, 9);
        BOOST_CHECK(test2);
    }
    {
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::AlphaNumeric;
        BOOST_CHECK(test1);

        BOOST_CHECK(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "bar";
        BOOST_CHECK(test2);
    }
}

BOOST_AUTO_TEST_CASE( semver_lexer_lexer_components_test )
{
    using semver::parser::Lexer;
    using semver::parser::Token;

    {
        Lexer lexer{"42"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::Numeric;
        BOOST_CHECK(test1);

        BOOST_CHECK(std::holds_alternative<Token::numeric_type>(token.component));
        const bool test2 = std::get<Token::numeric_type>(token.component) == 42;
        BOOST_CHECK(test2);
    }
    {
        Lexer lexer{"0"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::Numeric;
        BOOST_CHECK(test1);

        BOOST_CHECK(std::holds_alternative<Token::numeric_type>(token.component));
        const bool test2 = std::get<Token::numeric_type>(token.component) == 0;
        BOOST_CHECK(test2);
    }
    {
        Lexer lexer{"01"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::AlphaNumeric;
        BOOST_CHECK(test1);

        BOOST_CHECK(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "01";
        BOOST_CHECK(test2);
    }
    {
        Lexer lexer{"5885644aa"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::AlphaNumeric;
        BOOST_CHECK(test1);

        BOOST_CHECK(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "5885644aa";
        BOOST_CHECK(test2);
    }
    {
        Lexer lexer{"beta2"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Token::AlphaNumeric;
        BOOST_CHECK(test1);

        BOOST_CHECK(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "beta2";
        BOOST_CHECK(test2);
    }
    {
        Lexer lexer{"beta.2"};
        {
            const Token token = lexer.next();
            const bool test1 = token.kind == Token::AlphaNumeric;
            BOOST_CHECK(test1);

            BOOST_CHECK(std::holds_alternative<Token::alphanumeric_type>(token.component));
            const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "beta";
            BOOST_CHECK(test2);
        }
        {
            const Token token = lexer.next();
            const bool test = token.kind == Token::Dot;
            BOOST_CHECK(test);
        }
        {
            const Token token = lexer.next();
            const bool test1 = token.kind == Token::Numeric;
            BOOST_CHECK(test1);

            BOOST_CHECK(std::holds_alternative<Token::numeric_type>(token.component));
            const bool test2 = std::get<Token::numeric_type>(token.component) == 2;
            BOOST_CHECK(test2);
        }
    }
}

BOOST_AUTO_TEST_CASE( semver_lexer_is_wildcard_test )
{
    using semver::parser::Token;
    static_assert( Token{Token::Star}.is_whildcard() );
    static_assert( Token(Token::AlphaNumeric, "x").is_whildcard() );
    static_assert( Token(Token::AlphaNumeric, "X").is_whildcard() );
    static_assert( !Token(Token::AlphaNumeric, "other").is_whildcard() );
}

BOOST_AUTO_TEST_CASE( semver_lexer_str_to_int_test )
{
    using semver::parser::str_to_uint;
    static_assert( str_to_uint("123").value() == 123 );
    static_assert( !static_cast<bool>(str_to_uint("abc")) );
    static_assert( !static_cast<bool>(str_to_uint("12a")) );
}
