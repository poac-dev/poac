#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/util/semver/parser/token.hpp>

BOOST_AUTO_TEST_CASE( semver_parser_token_token_test )
{
    using semver::parser::Token;
    {
        constexpr Token token{ Token::Eq };
        static_assert( token == Token::Eq );
        static_assert( std::holds_alternative<std::monostate>(token.component) );
    }
    {
        constexpr Token token{ Token::Whitespace, 0, 2 };
        static_assert( token.kind == Token::Whitespace );
        static_assert( token.component == Token::variant_type(std::make_pair(0, 2)) );
    }
    {
        constexpr Token token{ Token::Numeric, 0 };
        static_assert( token.kind == Token::Numeric );
        static_assert( token.component == Token::variant_type(0) );
    }
    {
        constexpr Token token{ Token::AlphaNumeric, "beta.2" };
        static_assert( token.kind == Token::AlphaNumeric );
        static_assert( token.component == Token::variant_type("beta.2") );
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
    static_assert( Token( Token::Whitespace, 0, 2 ).is_whitespace() );
    static_assert( !Token( Token::Gt ).is_whitespace() );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_is_simple_token_test )
{
    using semver::parser::Token;
    static_assert( Token( Token::Eq ).is_simple_token() );
    static_assert( Token( Token::Gt ).is_simple_token() );
    static_assert( Token( Token::Lt ).is_simple_token() );
    static_assert( Token( Token::LtEq ).is_simple_token() );
    static_assert( Token( Token::GtEq ).is_simple_token() );
    static_assert( Token( Token::Caret ).is_simple_token() );
    static_assert( Token( Token::Tilde ).is_simple_token() );
    static_assert( Token( Token::Star ).is_simple_token() );
    static_assert( Token( Token::Dot ).is_simple_token() );
    static_assert( Token( Token::Comma ).is_simple_token() );
    static_assert( Token( Token::Hyphen ).is_simple_token() );
    static_assert( Token( Token::Plus ).is_simple_token() );
    static_assert( Token( Token::Or ).is_simple_token() );
    static_assert( Token( Token::Unexpected ).is_simple_token() );
    static_assert( !Token( Token::Numeric, 3 ).is_simple_token() );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_is_wildcard_test )
{
    using semver::parser::Token;
    static_assert( Token( Token::Star ).is_whildcard() );
    static_assert( Token( Token::AlphaNumeric, "x").is_whildcard() );
    static_assert( Token( Token::AlphaNumeric, "X").is_whildcard() );
    static_assert( !Token( Token::AlphaNumeric, "other").is_whildcard() );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_eq_test )
{
    using semver::parser::Token;
    static_assert( Token( Token::Star ) == Token::Star );
    static_assert( Token::Star == Token( Token::Star ) );
    static_assert( Token( Token::Star ) == Token( Token::Star ) );
    static_assert( Token( Token::Whitespace, 0, 2 ) == Token( Token::Whitespace, 0, 2 ) );
    static_assert( Token( Token::Numeric, 0 ) == Token( Token::Numeric, 0 ) );
    static_assert( Token( Token::AlphaNumeric, "x") == Token( Token::AlphaNumeric, "x") );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_neq_test )
{
    using semver::parser::Token;
    static_assert( Token( Token::Star ) != Token::Eq );
    static_assert( Token::Star != Token( Token::Eq ) );
    static_assert( Token( Token::Eq ) != Token( Token::Star ) );
    static_assert( Token( Token::Whitespace, 0, 2 ) != Token::Whitespace );
    static_assert( Token::Whitespace != Token( Token::Whitespace, 0, 7 ) );
    static_assert( Token::Numeric != Token( Token::Numeric, 0 ) );
    static_assert( Token( Token::Numeric, 8 ) != Token( Token::Numeric, 0 ) );
    static_assert( Token( Token::AlphaNumeric, "x") != Token( Token::AlphaNumeric, "X") );
    static_assert( Token( Token::AlphaNumeric, "x") != Token::AlphaNumeric );
    static_assert( Token::Numeric != Token( Token::AlphaNumeric, "0")  );
    static_assert( Token( Token::Numeric, 0 ) != Token( Token::AlphaNumeric, "0")  );
}
