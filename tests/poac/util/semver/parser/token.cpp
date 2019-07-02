#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/util/semver/parser/token.hpp>

BOOST_AUTO_TEST_CASE( semver_parser_token_token_test )
{
    using semver::parser::Token;
    {
        constexpr Token token{ Token::Kind::Eq };
        static_assert( token == Token::Kind::Eq );
        static_assert( std::holds_alternative<std::monostate>(token.component) );
    }
    {
        constexpr Token token{ Token::Kind::Whitespace, 0, 2 };
        static_assert( token.kind == Token::Kind::Whitespace );
        static_assert( token.component == Token::variant_type(std::make_pair(0, 2)) );
    }
    {
        constexpr Token token{ Token::Kind::Numeric, 0 };
        static_assert( token.kind == Token::Kind::Numeric );
        static_assert( token.component == Token::variant_type(0) );
    }
    {
        constexpr Token token{ Token::Kind::AlphaNumeric, "beta.2" };
        static_assert( token.kind == Token::Kind::AlphaNumeric );
        static_assert( token.component == Token::variant_type("beta.2") );
    }
    {
        BOOST_CHECK_THROW(
                Token( Token::Kind::AlphaNumeric, 0, 2 ),
                std::invalid_argument
        );
        BOOST_CHECK_THROW(
                Token( Token::Kind::Whitespace, 0 ),
                std::invalid_argument
        );
        BOOST_CHECK_THROW(
                Token( Token::Kind::Numeric, "beta.2" ),
                std::invalid_argument
        );
    }
}

BOOST_AUTO_TEST_CASE( semver_parser_token_is_whitespace_test )
{
    using semver::parser::Token;
    static_assert( Token( Token::Kind::Whitespace, 0, 2 ).is_whitespace() );
    static_assert( !Token( Token::Kind::Gt ).is_whitespace() );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_is_simple_token_test )
{
    using semver::parser::Token;
    static_assert( Token( Token::Kind::Eq ).is_simple_token() );
    static_assert( Token( Token::Kind::Gt ).is_simple_token() );
    static_assert( Token( Token::Kind::Lt ).is_simple_token() );
    static_assert( Token( Token::Kind::LtEq ).is_simple_token() );
    static_assert( Token( Token::Kind::GtEq ).is_simple_token() );
    static_assert( Token( Token::Kind::Caret ).is_simple_token() );
    static_assert( Token( Token::Kind::Tilde ).is_simple_token() );
    static_assert( Token( Token::Kind::Star ).is_simple_token() );
    static_assert( Token( Token::Kind::Dot ).is_simple_token() );
    static_assert( Token( Token::Kind::Comma ).is_simple_token() );
    static_assert( Token( Token::Kind::Hyphen ).is_simple_token() );
    static_assert( Token( Token::Kind::Plus ).is_simple_token() );
    static_assert( Token( Token::Kind::Or ).is_simple_token() );
    static_assert( Token( Token::Kind::Unexpected ).is_simple_token() );
    static_assert( !Token( Token::Kind::Numeric, 3 ).is_simple_token() );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_is_wildcard_test )
{
    using semver::parser::Token;
    static_assert( Token( Token::Kind::Star ).is_whildcard() );
    static_assert( Token( Token::Kind::AlphaNumeric, "x").is_whildcard() );
    static_assert( Token( Token::Kind::AlphaNumeric, "X").is_whildcard() );
    static_assert( !Token( Token::Kind::AlphaNumeric, "other").is_whildcard() );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_eq_test )
{
    using semver::parser::Token;
    static_assert( Token( Token::Kind::Star ) == Token::Kind::Star );
    static_assert( Token::Kind::Star == Token( Token::Kind::Star ) );
    static_assert( Token( Token::Kind::Star ) == Token( Token::Kind::Star ) );
    static_assert( Token( Token::Kind::Whitespace, 0, 2 ) == Token( Token::Kind::Whitespace, 0, 2 ) );
    static_assert( Token( Token::Kind::Numeric, 0 ) == Token( Token::Kind::Numeric, 0 ) );
    static_assert( Token( Token::Kind::AlphaNumeric, "x") == Token( Token::Kind::AlphaNumeric, "x") );
}

BOOST_AUTO_TEST_CASE( semver_parser_token_neq_test )
{
    using semver::parser::Token;
    static_assert( Token( Token::Kind::Star ) != Token::Kind::Eq );
    static_assert( Token::Kind::Star != Token( Token::Kind::Eq ) );
    static_assert( Token( Token::Kind::Eq ) != Token( Token::Kind::Star ) );
    static_assert( Token( Token::Kind::Whitespace, 0, 2 ) != Token::Kind::Whitespace );
    static_assert( Token::Kind::Whitespace != Token( Token::Kind::Whitespace, 0, 7 ) );
    static_assert( Token::Kind::Numeric != Token( Token::Kind::Numeric, 0 ) );
    static_assert( Token( Token::Kind::Numeric, 8 ) != Token( Token::Kind::Numeric, 0 ) );
    static_assert( Token( Token::Kind::AlphaNumeric, "x") != Token( Token::Kind::AlphaNumeric, "X") );
    static_assert( Token( Token::Kind::AlphaNumeric, "x") != Token::Kind::AlphaNumeric );
    static_assert( Token::Kind::Numeric != Token( Token::Kind::AlphaNumeric, "0")  );
    static_assert( Token( Token::Kind::Numeric, 0 ) != Token( Token::Kind::AlphaNumeric, "0")  );
}
