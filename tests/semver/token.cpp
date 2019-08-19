#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/predef.h>
#include <poac/util/semver/parser/token.hpp>

#if BOOST_OS_LINUX && BOOST_COMP_CLANG
#  define STATIC_ASSERT BOOST_CHECK
#else
#  define STATIC_ASSERT static_assert
#endif

BOOST_AUTO_TEST_CASE( semver_parser_token_token_test )
{
    using semver::parser::Token;
    {
        constexpr Token token{ Token::Eq };
        STATIC_ASSERT( token == Token::Eq );
        STATIC_ASSERT( std::holds_alternative<std::monostate>(token.component) );
    }
    {
        constexpr Token token{ Token::Whitespace, 0, 2 };
        STATIC_ASSERT( token.kind == Token::Whitespace );
        STATIC_ASSERT( token.component == Token::variant_type(std::make_pair(0, 2)) );
    }
    {
        constexpr Token token{ Token::Numeric, 0 };
        STATIC_ASSERT( token.kind == Token::Numeric );
        STATIC_ASSERT( token.component == Token::variant_type(0) );
    }
    {
        constexpr Token token{ Token::AlphaNumeric, "beta.2" };
        STATIC_ASSERT( token.kind == Token::AlphaNumeric );
        STATIC_ASSERT( token.component == Token::variant_type("beta.2") );
    }
    {
        BOOST_CHECK_THROW(
                Token( Token::AlphaNumeric, 0, 2 ),
                std::invalid_argument
        );
        BOOST_CHECK_THROW(
                Token( Token::Whitespace, 0 ),
                std::invalid_argument
        );
        BOOST_CHECK_THROW(
                Token( Token::Numeric, "beta.2" ),
                std::invalid_argument
        );
    }
}

BOOST_AUTO_TEST_CASE( semver_parser_token_is_whitespace_test )
{
    using semver::parser::Token;
    STATIC_ASSERT( Token( Token::Whitespace, 0, 2 ).is_whitespace() );
    STATIC_ASSERT( !Token( Token::Gt ).is_whitespace() );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_is_simple_token_test )
{
    using semver::parser::Token;
    STATIC_ASSERT( Token( Token::Eq ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Gt ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Lt ).is_simple_token() );
    STATIC_ASSERT( Token( Token::LtEq ).is_simple_token() );
    STATIC_ASSERT( Token( Token::GtEq ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Caret ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Tilde ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Star ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Dot ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Comma ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Hyphen ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Plus ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Or ).is_simple_token() );
    STATIC_ASSERT( Token( Token::Unexpected ).is_simple_token() );
    STATIC_ASSERT( !Token( Token::Numeric, 3 ).is_simple_token() );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_is_wildcard_test )
{
    using semver::parser::Token;
    STATIC_ASSERT( Token( Token::Star ).is_whildcard() );
    STATIC_ASSERT( Token( Token::AlphaNumeric, "x").is_whildcard() );
    STATIC_ASSERT( Token( Token::AlphaNumeric, "X").is_whildcard() );
    STATIC_ASSERT( !Token( Token::AlphaNumeric, "other").is_whildcard() );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_eq_test )
{
    using semver::parser::Token;
    STATIC_ASSERT( Token( Token::Star ) == Token::Star );
    STATIC_ASSERT( Token::Star == Token( Token::Star ) );
    STATIC_ASSERT( Token( Token::Star ) == Token( Token::Star ) );
    STATIC_ASSERT( Token( Token::Whitespace, 0, 2 ) == Token( Token::Whitespace, 0, 2 ) );
    STATIC_ASSERT( Token( Token::Numeric, 0 ) == Token( Token::Numeric, 0 ) );
    STATIC_ASSERT( Token( Token::AlphaNumeric, "x") == Token( Token::AlphaNumeric, "x") );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_neq_test )
{
    using semver::parser::Token;
    STATIC_ASSERT( Token( Token::Star ) != Token::Eq );
    STATIC_ASSERT( Token::Star != Token( Token::Eq ) );
    STATIC_ASSERT( Token( Token::Eq ) != Token( Token::Star ) );
    STATIC_ASSERT( Token( Token::Whitespace, 0, 2 ) != Token::Whitespace );
    STATIC_ASSERT( Token::Whitespace != Token( Token::Whitespace, 0, 7 ) );
    STATIC_ASSERT( Token::Numeric != Token( Token::Numeric, 0 ) );
    STATIC_ASSERT( Token( Token::Numeric, 8 ) != Token( Token::Numeric, 0 ) );
    STATIC_ASSERT( Token( Token::AlphaNumeric, "x") != Token( Token::AlphaNumeric, "X") );
    STATIC_ASSERT( Token( Token::AlphaNumeric, "x") != Token::AlphaNumeric );
    STATIC_ASSERT( Token::Numeric != Token( Token::AlphaNumeric, "0")  );
    STATIC_ASSERT( Token( Token::Numeric, 0 ) != Token( Token::AlphaNumeric, "0")  );
}
