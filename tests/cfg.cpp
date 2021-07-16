#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>
#include <boost/predef.h>
#include <string>
#include <memory>
#include <vector>
#include <poac/util/cfg.hpp>
#include "util/macros.hpp"

BOOST_AUTO_TEST_CASE( poac_util_cfg_parse_good_test )
{
    using poac::util::cfg::parse;
    using poac::util::cfg::ident_error;
    using poac::util::cfg::syntax_error;

    BOOST_CHECK_THROW_MSG(
        parse("foo"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "foo\n"
        "^-- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse("_bar"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "_bar\n"
        "^--- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse(" foo"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        " foo\n"
        " ^-- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse(" foo  "),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        " foo  \n"
        " ^-- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse(" foo  = \"bar\""),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        " foo  = \"bar\"\n"
        " ^-- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse("foo=\"\""),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "foo=\"\"\n"
        "^-- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse(" foo=\"3\"      "),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        " foo=\"3\"      \n"
        " ^-- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse("foo = \"3 e\""),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "foo = \"3 e\"\n"
        "^-- unknown identify"
    );

    BOOST_CHECK_THROW_MSG(
        parse("os"),
        syntax_error,
        "cfg syntax error\n"
        "os\n"
        "   ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse("_os"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "_os\n"
        "^-- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse(" os"),
        syntax_error,
        "cfg syntax error\n"
        " os\n"
        "    ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse(" os  "),
        syntax_error,
        "cfg syntax error\n"
        " os  \n"
        "    ^ expected operator"
    );
    BOOST_CHECK_NO_THROW( parse(" os  = \"bar\"") );
    BOOST_CHECK_NO_THROW( parse("os=\"\"") );
    BOOST_CHECK_NO_THROW( parse(" os=\"3\"      ") );
    BOOST_CHECK_NO_THROW( parse("os = \"3 e\"") );

    BOOST_CHECK_THROW_MSG(
        parse("all()"),
        syntax_error,
        "cfg syntax error\n"
        "all()\n"
        "      ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(a)"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "all(a)\n"
        "    ^ unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(a, b)"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "all(a, b)\n"
        "    ^ unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(a, )"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "all(a, )\n"
        "    ^ unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse("not(a = \"b\")"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "not(a = \"b\")\n"
        "    ^ unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse("not(all(a))"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "not(all(a))\n"
        "        ^ unknown identify"
    );

    BOOST_CHECK_THROW_MSG(
        parse("all()"),
        syntax_error,
        "cfg syntax error\n"
        "all()\n"
        "      ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(os)"),
        syntax_error,
        "cfg syntax error\n"
        "all(os)\n"
        "       ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(os, compiler)"),
        syntax_error,
        "cfg syntax error\n"
        "all(os, compiler)\n"
        "       ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(os, )"),
        syntax_error,
        "cfg syntax error\n"
        "all(os, )\n"
        "       ^ expected operator"
    );
    BOOST_CHECK_NO_THROW( parse("not(os = \"b\")") );
    BOOST_CHECK_THROW_MSG(
        parse("not(all(os))"),
        syntax_error,
        "cfg syntax error\n"
        "not(all(os))\n"
        "           ^ expected operator"
    );

    BOOST_CHECK_NO_THROW( parse("cfg(not(os = \"foo\"))") );
    BOOST_CHECK_NO_THROW( parse("all(os = \"foo\", os = \"bar\")") );
    BOOST_CHECK_NO_THROW( parse("any(os = \"foo\", os = \"bar\")") );
}

BOOST_AUTO_TEST_CASE( poac_util_cfg_parse_bad_test )
{
    using poac::util::cfg::parse;
    using poac::util::cfg::string_error;
    using poac::util::cfg::ident_error;
    using poac::util::cfg::operator_error;
    using poac::util::cfg::expression_error;
    using poac::util::cfg::syntax_error;

    BOOST_CHECK_THROW_MSG(
        parse(" "),
        expression_error,
        "expected start of a cfg expression"
    );
    BOOST_CHECK_THROW_MSG(
        parse(" all"),
        syntax_error,
        "cfg syntax error\n"
        " all\n"
        "    ^ expected '(', but cfg expression ended"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(os"),
        syntax_error,
        "cfg syntax error\n"
        "all(os\n"
        "       ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse("not"),
        syntax_error,
        "cfg syntax error\n"
        "not\n"
        "   ^ expected '(', but cfg expression ended"
    );
    BOOST_CHECK_THROW_MSG(
        parse("not(os"),
        syntax_error,
        "cfg syntax error\n"
        "not(os\n"
        "       ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse("os "),
        syntax_error,
        "cfg syntax error\n"
        "os \n"
        "   ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse("os = "),
        syntax_error,
        "cfg syntax error\n"
        "os = \n"
        "    ^ expected a string, but cfg expression ended"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(not())"),
        syntax_error,
        "cfg syntax error\n"
        "all(not())\n"
        "          ^ expected operator"
    );
    BOOST_CHECK_THROW_MSG(
        parse("foo(a)"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "foo(a)\n"
        "^-- unknown identify"
    );

    BOOST_CHECK_THROW_MSG(
        parse("os = \"foo"),
        string_error,
        "missing terminating '\"' character\n"
        "os = \"foo\n"
        "     ^--- unterminated string"
    );
    BOOST_CHECK_THROW_MSG(
        parse("os < \"foo\""),
        operator_error,
        "cfg operator error\n"
        "os < \"foo\"\n"
        "   ^ cannot be specified except os_version"
    );
    BOOST_CHECK_THROW_MSG(
        parse("os <= \"foo\""),
        operator_error,
        "cfg operator error\n"
        "os <= \"foo\"\n"
        "   ^- cannot be specified except os_version"
    );
    BOOST_CHECK_THROW_MSG(
            parse("os > \"foo\""),
            operator_error,
            "cfg operator error\n"
            "os > \"foo\"\n"
            "   ^ cannot be specified except os_version"
    );
    BOOST_CHECK_THROW_MSG(
            parse("os >= \"foo\""),
            operator_error,
            "cfg operator error\n"
            "os >= \"foo\"\n"
            "   ^- cannot be specified except os_version"
    );
    BOOST_CHECK_NO_THROW( parse("os_version < \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os_version <= \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os_version > \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os_version >= \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os_version = \"foo\"") );

    BOOST_CHECK_THROW_MSG(
        parse("all(os = \"foo\""),
        syntax_error,
        "cfg syntax error\n"
        "all(os = \"foo\"\n"
        "              ^ expected ')', but cfg expression ended"
    );

    BOOST_CHECK_THROW_MSG(
        parse("and()"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "and()\n"
        "^-- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse("or()"),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "or()\n"
        "^- unknown identify"
    );
    BOOST_CHECK_NO_THROW( parse("compiler = \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("arch = \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("feature = \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os = \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("platform = \"foo\"") );
    BOOST_CHECK_THROW_MSG(
        parse("foo = \"bar\""),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "foo = \"bar\"\n"
        "^-- unknown identify"
    );
    BOOST_CHECK_THROW_MSG(
        parse("3compiler = \"bar\""),
        ident_error,
        "cfg expected parenthesis, comma, identifier, or string\n"
        "3compiler = \"bar\"\n"
        "^ unexpected character"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(compiler = os)"),
        syntax_error,
        "cfg syntax error\n"
        "all(compiler = os)\n"
        "               ^- expected a string"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all compiler = \"bar\")"),
        syntax_error,
        "cfg syntax error\n"
        "all compiler = \"bar\")\n"
        "   ^ excepted '(' after `all`"
    );
    BOOST_CHECK_THROW_MSG(
        parse("all(not(compiler = \"foo\", os = \"bar\")"),
        syntax_error,
        "cfg syntax error\n"
        "all(not(compiler = \"foo\", os = \"bar\")\n"
        "                        ^ excepted ')'"
    );
}

// struct Token
BOOST_AUTO_TEST_CASE( poac_util_cfg_token_bad_construct_test )
{
    using poac::util::cfg::Token;

    BOOST_CHECK_THROW( Token{Token::String}, std::invalid_argument );
    BOOST_CHECK_THROW( Token{Token::Ident}, std::invalid_argument );
    BOOST_CHECK_THROW( Token(Token::Comma, "foo"), std::invalid_argument );
    BOOST_CHECK_THROW( Token(Token::GtEq, Token::ident::os), std::invalid_argument );
}

// std::string to_string(Token::ident ident)
BOOST_AUTO_TEST_CASE( poac_util_cfg_token_ident_to_string_test )
{
    using poac::util::cfg::Token;
    using poac::util::cfg::to_string;

    BOOST_CHECK( to_string(Token::ident::cfg) == "cfg" );
    BOOST_CHECK( to_string(Token::ident::not_) == "not" );
    BOOST_CHECK( to_string(Token::ident::all) == "all" );
    BOOST_CHECK( to_string(Token::ident::any) == "any" );
    BOOST_CHECK( to_string(Token::ident::compiler) == "compiler" );
    BOOST_CHECK( to_string(Token::ident::arch) == "arch" );
    BOOST_CHECK( to_string(Token::ident::feature) == "feature" );
    BOOST_CHECK( to_string(Token::ident::os) == "os" );
    BOOST_CHECK( to_string(Token::ident::os_version) == "os_version" );
    BOOST_CHECK( to_string(Token::ident::platform) == "platform" );
}

// std::ostream& operator<<(std::ostream& os, const Token& token)
BOOST_AUTO_TEST_CASE( poac_util_cfg_token_ostream_test )
{
    using poac::util::cfg::Token;
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::LeftParen};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "left_paren: (" ) );
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::RightParen};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "right_paren: )" ) );
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Comma};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "comma: ," ) );
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Equals};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "equals: =" ) );
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Gt};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "gt: >" ) );
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::GtEq};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "gteq: >=" ) );
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Lt};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "lt: <" ) );
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::LtEq};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "lteq: <=" ) );
    }
    {
        boost::test_tools::output_test_stream output;
        std::string s = "foo";
        output << Token{Token::String, s};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "string: " + s ) );
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Ident, Token::ident::os};
        BOOST_CHECK( !output.is_empty( false ) );
        BOOST_CHECK( output.is_equal( "ident: " + to_string(Token::ident::os) ) );
    }
}

// Ident from_token_ident(Token::ident ident)
BOOST_AUTO_TEST_CASE( poac_util_cfg_cfg_from_token_ident_test )
{
    using poac::util::cfg::Cfg;
    using poac::util::cfg::Token;

    BOOST_CHECK_THROW(
        Cfg(Token::ident::all, Cfg::Op::Lt, "foo"),
        std::invalid_argument
    );
}

// struct CfgExpr
BOOST_AUTO_TEST_CASE( poac_util_cfg_cfgexpr_bad_construct_test )
{
    using poac::util::cfg::CfgExpr;
    using poac::util::cfg::Cfg;
    using poac::util::cfg::Token;

    const auto c = Cfg(Token::ident::compiler, Cfg::Op::Equals, "foo");
    BOOST_CHECK_THROW( CfgExpr(CfgExpr::cfg, c), std::invalid_argument );

    std::vector<CfgExpr> e;
    e.emplace_back(CfgExpr(CfgExpr::value, c));
    BOOST_CHECK_THROW(
        CfgExpr(
            CfgExpr::not_, std::move(e)
        ),
        std::invalid_argument
    );

    BOOST_CHECK_THROW(
        CfgExpr(
            CfgExpr::all,
            std::make_unique<CfgExpr>(
                CfgExpr(CfgExpr::value, c)
            )
        ),
        std::invalid_argument
    );
}

#if BOOST_OS_LINUX || BOOST_OS_MACOS
BOOST_AUTO_TEST_CASE( poac_util_cfg_cfgexpr_match_test )
{
    using poac::util::cfg::parse;

    BOOST_CHECK( !parse("cfg(not(os = \"unix\"))").match() );
    BOOST_CHECK( parse("cfg(not(not(os = \"unix\")))").match() );
    BOOST_CHECK( parse("cfg(any(os = \"linux\", os = \"macos\")))").match() );
    BOOST_CHECK( !parse("cfg(all(os = \"linux\", os = \"macos\")))").match() );
    BOOST_CHECK( parse("cfg(all(os = \"unix\", os = \"unix\")))").match() );

    // Always return false (unimplemented)
    BOOST_CHECK( !parse("cfg(compiler = \"gcc\")").match() );
    BOOST_CHECK( !parse("cfg(os_version > \"14\")").match() );

#if BOOST_ARCH_X86
    BOOST_CHECK( parse("cfg(arch = \"x86\")").match() );
#  if BOOST_ARCH_X86_64
    BOOST_CHECK( parse("cfg(arch = \"x86_64\")").match() );
#  endif
#endif

//#ifdef __SSE__
//    BOOST_CHECK( parse("cfg(feature = \"sse\")").match() );
//#endif

#if !BOOST_PLAT_MINGW
    BOOST_CHECK( parse("cfg(not(platform = \"mingw\"))").match() );
#endif

//#if BOOST_ARCH_X86_64 && defined(__SSE__) && !BOOST_PLAT_MINGW
//    BOOST_CHECK(
//        parse("cfg(all(os = \"unix\", "
//                      "arch = \"x86_64\", "
//                      "feature = \"x86_64\", "
//                      "not(platform = \"mingw\")"
//                   "))").match()
//    );
//#endif

    using poac::util::cfg::CfgExpr;
    using poac::util::cfg::Cfg;
    using poac::util::cfg::Token;
    CfgExpr test_case{
        CfgExpr::value,
        Cfg{
            Token::ident::compiler,
            Cfg::Op::Equals,
            "foo"
        }
    };
    test_case.kind = static_cast<CfgExpr::Kind>(99);
    BOOST_CHECK_THROW(test_case.match(), std::logic_error);
}
#endif

// Token::Kind to_kind(std::string_view kind)
BOOST_AUTO_TEST_CASE( poac_util_cfg_to_kind_test )
{
    using poac::util::cfg::Token;
    using poac::util::cfg::to_kind;

    static_assert( to_kind("(") == Token::LeftParen );
    static_assert( to_kind(")") == Token::RightParen );
    static_assert( to_kind(",") == Token::Comma );
    static_assert( to_kind("=") == Token::Equals );
    static_assert( to_kind(">") == Token::Gt );
    static_assert( to_kind(">=") == Token::GtEq );
    static_assert( to_kind("<") == Token::Lt );
    static_assert( to_kind("<=") == Token::LtEq );
    BOOST_CHECK_THROW( to_kind("unknown"), poac::util::cfg::exception );
}

// std::string to_string(Token::ident ident)
BOOST_AUTO_TEST_CASE( poac_util_cfg_to_string_test )
{
    using poac::util::cfg::Token;
    using poac::util::cfg::to_string;

    BOOST_CHECK( to_string(Token::ident::cfg) == "cfg" );
    BOOST_CHECK( to_string(Token::ident::not_) == "not" );
    BOOST_CHECK( to_string(Token::ident::all) == "all" );
    BOOST_CHECK( to_string(Token::ident::any) == "any" );
    BOOST_CHECK( to_string(Token::ident::compiler) == "compiler" );
    BOOST_CHECK( to_string(Token::ident::arch) == "arch" );
    BOOST_CHECK( to_string(Token::ident::feature) == "feature" );
    BOOST_CHECK( to_string(Token::ident::os) == "os" );
    BOOST_CHECK( to_string(Token::ident::os_version) == "os_version" );
    BOOST_CHECK( to_string(Token::ident::platform) == "platform" );
    BOOST_CHECK_THROW( to_string(static_cast<Token::ident>(99)), std::logic_error );
}

// std::ostream& operator<<(std::ostream& os, const Token& token)
BOOST_AUTO_TEST_CASE( poac_util_cfg_operator_out_test )
{
    using poac::util::cfg::Token;
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::LeftParen};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("left_paren: ("));
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::RightParen};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("right_paren: )"));
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Comma};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("comma: ,"));
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Equals};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("equals: ="));
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Gt};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("gt: >"));
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::GtEq};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("gteq: >="));
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Lt};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("lt: <"));
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::LtEq};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("lteq: <="));
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::String, "test_string"};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("string: test_string"));
    }
    {
        boost::test_tools::output_test_stream output;
        output << Token{Token::Ident, Token::ident::cfg};
        BOOST_CHECK(!output.is_empty(false));
        BOOST_CHECK(output.is_equal("ident: cfg"));
    }
    {
        boost::test_tools::output_test_stream output;
        BOOST_CHECK_THROW(
            poac::util::cfg::operator<<(output, Token{static_cast<Token::Kind>(99)}),
            std::logic_error
        );
    }
}
