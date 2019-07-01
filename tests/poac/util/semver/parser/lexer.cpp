#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/util/semver/parser/lexer.hpp>

BOOST_AUTO_TEST_CASE( semver_lexer_token_test )
{
    using semver::Token;
    using semver::Kind;

    {
        constexpr Token t{ Kind::Eq };
        static_assert( t.kind == Kind::Eq );
        static_assert( std::holds_alternative<std::monostate>(t.component) );
    }
    {
        constexpr Token t{ Kind::Whitespace, 0, 2 };
        static_assert( t.kind == Kind::Whitespace );
        static_assert( std::holds_alternative<Token::whitespace_type>(t.component) );
        static_assert( std::get<Token::whitespace_type>(t.component) == std::make_tuple(0, 2) );
    }
    {
        constexpr Token t{ Kind::Numeric, 0 };
        static_assert( t.kind == Kind::Numeric );
        static_assert( std::holds_alternative<Token::numeric_type>(t.component) );
        static_assert( std::get<Token::numeric_type>(t.component) == 0 );
    }
    {
        constexpr Token t{ Kind::AlphaNumeric, "hoge" };
        static_assert( t.kind == Kind::AlphaNumeric );
        static_assert( std::holds_alternative<Token::alphanumeric_type>(t.component) );
    }
}

BOOST_AUTO_TEST_CASE( semver_lexer_simple_tokens_test )
{
    using semver::Lexer;
    using semver::Token;
    using semver::Kind;

    Lexer lexer{"=><<=>=^~*.,-+||"};
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Eq;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Gt;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Lt;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::LtEq;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::GtEq;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Caret;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Tilde;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Star;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Dot;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Comma;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Hyphen;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Plus;
        BOOST_TEST(test);
    }
    {
        const Token token = lexer.next();
        const bool test = token.kind == Kind::Or;
        BOOST_TEST(test);
    }
}

BOOST_AUTO_TEST_CASE( semver_lexer_lexer_whitespace_test )
{
    using semver::Lexer;
    using semver::Token;
    using semver::Kind;

    Lexer lexer{"  foo \t\n\rbar"};
    {
        const Token token = lexer.next();
        const bool test1 = token.kind == Kind::Whitespace;
        BOOST_TEST(test1);

        BOOST_TEST(std::holds_alternative<Token::whitespace_type>(token.component));
        const bool test2 = std::get<Token::whitespace_type>(token.component) == std::make_tuple(0, 2);
        BOOST_TEST(test2);
    }
    {
        const Token token = lexer.next();
        const bool test1 = token.kind == Kind::AlphaNumeric;
        BOOST_TEST(test1);

        BOOST_TEST(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "foo";
        BOOST_TEST(test2);
    }
    {
        const Token token = lexer.next();
        const bool test1 = token.kind == Kind::Whitespace;
        BOOST_TEST(test1);

        BOOST_TEST(std::holds_alternative<Token::whitespace_type>(token.component));
        const bool test2 = std::get<Token::whitespace_type>(token.component) == std::make_tuple(5, 9);
        BOOST_TEST(test2);
    }
    {
        const Token token = lexer.next();
        const bool test1 = token.kind == Kind::AlphaNumeric;
        BOOST_TEST(test1);

        BOOST_TEST(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "bar";
        BOOST_TEST(test2);
    }
}

BOOST_AUTO_TEST_CASE( semver_lexer_lexer_components_test )
{
    using semver::Lexer;
    using semver::Token;
    using semver::Kind;

    {
        Lexer lexer{"42"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Kind::Numeric;
        BOOST_TEST(test1);

        BOOST_TEST(std::holds_alternative<Token::numeric_type>(token.component));
        const bool test2 = std::get<Token::numeric_type>(token.component) == 42;
        BOOST_TEST(test2);
    }
    {
        Lexer lexer{"0"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Kind::Numeric;
        BOOST_TEST(test1);

        BOOST_TEST(std::holds_alternative<Token::numeric_type>(token.component));
        const bool test2 = std::get<Token::numeric_type>(token.component) == 0;
        BOOST_TEST(test2);
    }
    {
        Lexer lexer{"01"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Kind::AlphaNumeric;
        BOOST_TEST(test1);

        BOOST_TEST(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "01";
        BOOST_TEST(test2);
    }
    {
        Lexer lexer{"5885644aa"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Kind::AlphaNumeric;
        BOOST_TEST(test1);

        BOOST_TEST(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "5885644aa";
        BOOST_TEST(test2);
    }
    {
        Lexer lexer{"beta2"};
        const Token token = lexer.next();
        const bool test1 = token.kind == Kind::AlphaNumeric;
        BOOST_TEST(test1);

        BOOST_TEST(std::holds_alternative<Token::alphanumeric_type>(token.component));
        const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "beta2";
        BOOST_TEST(test2);
    }
    {
        Lexer lexer{"beta.2"};
        {
            const Token token = lexer.next();
            const bool test1 = token.kind == Kind::AlphaNumeric;
            BOOST_TEST(test1);

            BOOST_TEST(std::holds_alternative<Token::alphanumeric_type>(token.component));
            const bool test2 = std::get<Token::alphanumeric_type>(token.component) == "beta";
            BOOST_TEST(test2);
        }
        {
            const Token token = lexer.next();
            const bool test = token.kind == Kind::Dot;
            BOOST_TEST(test);
        }
        {
            const Token token = lexer.next();
            const bool test1 = token.kind == Kind::Numeric;
            BOOST_TEST(test1);

            BOOST_TEST(std::holds_alternative<Token::numeric_type>(token.component));
            const bool test2 = std::get<Token::numeric_type>(token.component) == 2;
            BOOST_TEST(test2);
        }
    }
}

BOOST_AUTO_TEST_CASE( semver_lexer_is_wildcard_test )
{
    using semver::Token;
    using semver::Kind;

    static_assert( Token{Kind::Star}.is_whildcard() );
    static_assert( Token(Kind::AlphaNumeric, "x").is_whildcard() );
    static_assert( Token(Kind::AlphaNumeric, "X").is_whildcard() );
    static_assert( !Token(Kind::AlphaNumeric, "other").is_whildcard() );
}

BOOST_AUTO_TEST_CASE( semver_lexer_str_to_int_test )
{
    using semver::str_to_int;
    static_assert( str_to_int("123").value() == 123 );
    static_assert( !static_cast<bool>(str_to_int("abc")) );
    static_assert( !static_cast<bool>(str_to_int("12a")) );
}
