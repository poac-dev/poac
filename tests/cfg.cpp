#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/util/cfg.hpp>
#include <poac/core/except.hpp>

BOOST_AUTO_TEST_CASE( poac_util_cfg_parse_good_test )
{
    using poac::util::cfg::parse;
    using poac::util::cfg::ident_error;
    using poac::util::cfg::syntax_error;

    BOOST_CHECK_THROW( parse("foo"), ident_error );
    BOOST_CHECK_THROW( parse("_bar"), ident_error );
    BOOST_CHECK_THROW( parse(" foo"), ident_error );
    BOOST_CHECK_THROW( parse(" foo  "), ident_error );
    BOOST_CHECK_THROW( parse(" foo  = \"bar\""), ident_error );
    BOOST_CHECK_THROW( parse("foo=\"\""), ident_error );
    BOOST_CHECK_THROW( parse(" foo=\"3\"      "), ident_error );
    BOOST_CHECK_THROW( parse("foo = \"3 e\""), ident_error );

    BOOST_CHECK_THROW( parse("os"), syntax_error );
    BOOST_CHECK_THROW( parse("_os"), ident_error );
    BOOST_CHECK_THROW( parse(" os"), syntax_error );
    BOOST_CHECK_THROW( parse(" os  "), syntax_error );
    BOOST_CHECK_NO_THROW( parse(" os  = \"bar\"") );
    BOOST_CHECK_NO_THROW( parse("os=\"\"") );
    BOOST_CHECK_NO_THROW( parse(" os=\"3\"      ") );
    BOOST_CHECK_NO_THROW( parse("os = \"3 e\"") );

    BOOST_CHECK_THROW( parse("all()"), syntax_error );
    BOOST_CHECK_THROW( parse("all(a)"), ident_error );
    BOOST_CHECK_THROW( parse("all(a, b)"), ident_error );
    BOOST_CHECK_THROW( parse("all(a, )"), ident_error );
    BOOST_CHECK_THROW( parse("not(a = \"b\")"), ident_error );
    BOOST_CHECK_THROW( parse("not(all(a))"), ident_error );

    BOOST_CHECK_THROW( parse("all()"), syntax_error );
    BOOST_CHECK_THROW( parse("all(os)"), syntax_error );
    BOOST_CHECK_THROW( parse("all(os, compiler)"), syntax_error );
    BOOST_CHECK_THROW( parse("all(os, )"), syntax_error );
    BOOST_CHECK_NO_THROW( parse("not(os = \"b\")") );
    BOOST_CHECK_THROW( parse("not(all(os))"), syntax_error );

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

    BOOST_CHECK_THROW( parse(" "), expression_error );
    BOOST_CHECK_THROW( parse(" all"), syntax_error );
    BOOST_CHECK_THROW( parse("all(os"), syntax_error );
    BOOST_CHECK_THROW( parse("not"), syntax_error );
    BOOST_CHECK_THROW( parse("not(os"), syntax_error );
    BOOST_CHECK_THROW( parse("os "), syntax_error );
    BOOST_CHECK_THROW( parse("os = "), syntax_error );
    BOOST_CHECK_THROW( parse("all(not())"), syntax_error );
    BOOST_CHECK_THROW( parse("foo(a)"), ident_error );

    BOOST_CHECK_THROW( parse("os = \"foo"), string_error );
    BOOST_CHECK_THROW( parse("os < \"foo\""), operator_error );
    BOOST_CHECK_THROW( parse("os <= \"foo\""), operator_error );
    BOOST_CHECK_THROW( parse("os > \"foo\""), operator_error );
    BOOST_CHECK_THROW( parse("os >= \"foo\""), operator_error );
    BOOST_CHECK_NO_THROW( parse("os_version < \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os_version <= \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os_version > \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os_version >= \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os_version = \"foo\"") );

    BOOST_CHECK_THROW( parse("all(os = \"foo\""), syntax_error );

    BOOST_CHECK_THROW( parse("and()"), ident_error );
    BOOST_CHECK_THROW( parse("or()"), ident_error );
    BOOST_CHECK_NO_THROW( parse("compiler = \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("arch = \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("feature = \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("os = \"foo\"") );
    BOOST_CHECK_NO_THROW( parse("platform = \"foo\"") );
    BOOST_CHECK_THROW( parse("foo = \"bar\""), ident_error );
}
