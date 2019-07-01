#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/util/semver.hpp>

BOOST_AUTO_TEST_CASE( semver_eq_test )
{
    using semver::Version;

    BOOST_TEST( Version("1.2.3") == "1.2.3" );
    BOOST_TEST( Version("1.2.3-alpha") == "1.2.3-alpha" );
    BOOST_TEST( Version("1.2.3+2013") == "1.2.3+2014" );
    BOOST_TEST( Version("1.2.3-alpha+2013") == "1.2.3-alpha+2014" );
    BOOST_TEST( Version("1.0.0-beta+exp.sha.5114f85") == "1.0.0-beta" );
}

BOOST_AUTO_TEST_CASE( semver_lt_test )
{
    using semver::Version;

    BOOST_TEST( Version("1.2.3") < "1.2.4" );
    BOOST_TEST( Version("1.2.3") < "1.3.3" );
    BOOST_TEST( Version("1.2.3") < "2.2.3" );
    BOOST_TEST( Version("1.2.3") < "1.3.3" );
    BOOST_TEST( !(Version("9.1.1") < "1.9.9") );
    BOOST_TEST( Version("1.2.3-alpha") < "1.2.3" );
    BOOST_TEST( Version("1.2.3-alpha") < "1.2.3-beta" );
    BOOST_TEST( Version("1.2.3-beta") < "1.2.3-pre" );
    BOOST_TEST( Version("1.2.3-beta+build") < "1.2.3-pre+build" );
    BOOST_TEST( Version("1.0.0-alpha") < "1.0.0-alpha.1" );
    BOOST_TEST( Version("1.0.0-alpha.1") < "1.0.0-alpha.beta" );
    BOOST_TEST( Version("1.0.0-alpha.beta") < "1.0.0-beta" );
    BOOST_TEST( Version("1.0.0-beta") < "1.0.0-beta.2" );
    BOOST_TEST( Version("1.0.0-beta.2") < "1.0.0-beta.11" );
    BOOST_TEST( Version("1.0.0-beta.11") < "1.0.0-rc.1" );
    BOOST_TEST( Version("1.0.0-rc.1") < "1.0.0" );
}

BOOST_AUTO_TEST_CASE( semver_lte_test )
{
    using semver::Version;

    BOOST_TEST( Version("1.2.3") <= "1.2.3" );
    BOOST_TEST( Version("1.2.3-alpha") <= "1.2.3-alpha" );
    BOOST_TEST( Version("1.2.3+2013") <= "1.2.3+2014" );
    BOOST_TEST( Version("1.2.3-alpha+2013") <= "1.2.3-alpha+2014" );
    BOOST_TEST( Version("1.0.0-beta+exp.sha.5114f85") <= "1.0.0-beta" );

    BOOST_TEST( Version("1.2.3") <= "1.2.4" );
    BOOST_TEST( Version("1.2.3") <= "1.3.3" );
    BOOST_TEST( Version("1.2.3") <= "2.2.3" );
    BOOST_TEST( Version("1.2.3") <= "1.3.3" );
    BOOST_TEST( !(Version("9.1.1") <= "1.9.9") );
    BOOST_TEST( Version("1.2.3-alpha") <= "1.2.3" );
    BOOST_TEST( Version("1.2.3-alpha") <= "1.2.3-beta" );
    BOOST_TEST( Version("1.2.3-beta") <= "1.2.3-pre" );
    BOOST_TEST( Version("1.2.3-beta+build") <= "1.2.3-pre+build" );
    BOOST_TEST( Version("1.0.0-alpha") <= "1.0.0-alpha.1" );
    BOOST_TEST( Version("1.0.0-alpha.1") <= "1.0.0-alpha.beta" );
    BOOST_TEST( Version("1.0.0-alpha.beta") <= "1.0.0-beta" );
    BOOST_TEST( Version("1.0.0-beta") <= "1.0.0-beta.2" );
    BOOST_TEST( Version("1.0.0-beta.2") <= "1.0.0-beta.11" );
    BOOST_TEST( Version("1.0.0-beta.11") <= "1.0.0-rc.1" );
    BOOST_TEST( Version("1.0.0-rc.1") <= "1.0.0" );
}

BOOST_AUTO_TEST_CASE( semver_gt_test )
{
    using semver::Version;

    BOOST_TEST( Version("1.2.4") > "1.2.3" );
    BOOST_TEST( Version("1.3.3") > "1.2.3" );
    BOOST_TEST( Version("2.2.3") > "1.2.3" );
    BOOST_TEST( Version("1.3.3") > "1.2.3" );
    BOOST_TEST( !(Version("1.9.9") > "9.1.1") );
    BOOST_TEST( Version("1.2.3") > "1.2.3-alpha" );
    BOOST_TEST( Version("1.2.3-beta") > "1.2.3-alpha" );
    BOOST_TEST( Version("1.2.3-pre") > "1.2.3-beta" );
    BOOST_TEST( Version("1.2.3-pre+build") > "1.2.3-beta+build" );
    BOOST_TEST( Version("1.0.0-alpha.1") > "1.0.0-alpha" );
    BOOST_TEST( Version("1.0.0-alpha.beta") > "1.0.0-alpha.1" );
    BOOST_TEST( Version("1.0.0-beta") > "1.0.0-alpha.beta" );
    BOOST_TEST( Version("1.0.0-beta.2") > "1.0.0-beta" );
    BOOST_TEST( Version("1.0.0-beta.11") > "1.0.0-beta.2" );
    BOOST_TEST( Version("1.0.0-rc.1") > "1.0.0-beta.11" );
    BOOST_TEST( Version("1.0.0") > "1.0.0-rc.1" );
}

BOOST_AUTO_TEST_CASE( semver_gte_test )
{
    using semver::Version;

    BOOST_TEST( Version("1.2.3") >= "1.2.3" );
    BOOST_TEST( Version("1.2.3-alpha") >= "1.2.3-alpha" );
    BOOST_TEST( Version("1.2.3+2013") >= "1.2.3+2014" );
    BOOST_TEST( Version("1.2.3-alpha+2013") >= "1.2.3-alpha+2014" );
    BOOST_TEST( Version("1.0.0-beta+exp.sha.5114f85") >= "1.0.0-beta" );

    BOOST_TEST( Version("1.2.4") >= "1.2.3" );
    BOOST_TEST( Version("1.3.3") >= "1.2.3" );
    BOOST_TEST( Version("2.2.3") >= "1.2.3" );
    BOOST_TEST( Version("1.3.3") >= "1.2.3" );
    BOOST_TEST( !(Version("1.9.9") >= "9.1.1") );
    BOOST_TEST( Version("1.2.3") >= "1.2.3-alpha" );
    BOOST_TEST( Version("1.2.3-beta") >= "1.2.3-alpha" );
    BOOST_TEST( Version("1.2.3-pre") >= "1.2.3-beta" );
    BOOST_TEST( Version("1.2.3-pre+build") >= "1.2.3-beta+build" );
    BOOST_TEST( Version("1.0.0-alpha.1") >= "1.0.0-alpha" );
    BOOST_TEST( Version("1.0.0-alpha.beta") >= "1.0.0-alpha.1" );
    BOOST_TEST( Version("1.0.0-beta") >= "1.0.0-alpha.beta" );
    BOOST_TEST( Version("1.0.0-beta.2") >= "1.0.0-beta" );
    BOOST_TEST( Version("1.0.0-beta.11") >= "1.0.0-beta.2" );
    BOOST_TEST( Version("1.0.0-rc.1") >= "1.0.0-beta.11" );
    BOOST_TEST( Version("1.0.0") >= "1.0.0-rc.1" );
}

// TODO: regex, Version constructor, get_version(), get_full(), Version exceptions
