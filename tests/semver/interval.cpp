#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/semver/interval.hpp>
#include <poac/util/semver/exception.hpp>

BOOST_AUTO_TEST_CASE( semver_satisfies_test )
{
    using semver::Interval;

    Interval interval(">=1.66.0 and <1.70.0");
    BOOST_CHECK( interval.satisfies("1.66.0") );
    BOOST_CHECK( interval.satisfies("1.67.0") );
    BOOST_CHECK( interval.satisfies("1.68.0") );
    BOOST_CHECK( interval.satisfies("1.69.0") );
    BOOST_CHECK( interval.satisfies("1.69.9") );
    BOOST_CHECK( !interval.satisfies("1.70.0") );
}

BOOST_AUTO_TEST_CASE( semver_satisfies_test2 )
{
    using semver::Interval;

    Interval interval(">=1.0.0-alpha and <1.0.0");
    BOOST_CHECK( interval.satisfies("1.0.0-alpha") );
    BOOST_CHECK( interval.satisfies("1.0.0-alpha.1") );
    BOOST_CHECK( interval.satisfies("1.0.0-alpha.beta") );
    BOOST_CHECK( interval.satisfies("1.0.0-beta") );
    BOOST_CHECK( interval.satisfies("1.0.0-beta.2") );
    BOOST_CHECK( interval.satisfies("1.0.0-beta.11") );
    BOOST_CHECK( interval.satisfies("1.0.0-rc.1") );
    BOOST_CHECK( !interval.satisfies("1.0.0") );
}

BOOST_AUTO_TEST_CASE( semver_is_wasteful_comparison_operation_test )
{
    using semver::Interval;
    using semver::invalid_interval_error;

    BOOST_CHECK_THROW( Interval("<2.0.0 and <1.0.0"), invalid_interval_error );
    BOOST_CHECK_THROW( Interval("<=2.0.0 and <=1.0.0"), invalid_interval_error );
    BOOST_CHECK_THROW( Interval("<2.0.0 and <=1.0.0"), invalid_interval_error );
    BOOST_CHECK_THROW( Interval("<=2.0.0 and <1.0.0"), invalid_interval_error );

    BOOST_CHECK_THROW( Interval("<1.0.0-alpha and <1.0.0"), invalid_interval_error );
    BOOST_CHECK_THROW( Interval("<1.0.0 and <1.0.0"), invalid_interval_error );

    BOOST_CHECK_THROW( Interval(">2.0.0 and >1.0.0"), invalid_interval_error );
    BOOST_CHECK_THROW( Interval(">=2.0.0 and >=1.0.0"), invalid_interval_error );
    BOOST_CHECK_THROW( Interval(">2.0.0 and >=1.0.0"), invalid_interval_error );
    BOOST_CHECK_THROW( Interval(">=2.0.0 and >1.0.0"), invalid_interval_error );

    BOOST_CHECK_THROW( Interval(">1.0.0-alpha and >1.0.0"), invalid_interval_error );
    BOOST_CHECK_THROW( Interval(">1.0.0 and >1.0.0"), invalid_interval_error );
}

BOOST_AUTO_TEST_CASE( semver_is_bounded_interval_test )
{
    using semver::Interval;
    using semver::strange_interval_error;

    BOOST_CHECK_THROW( Interval("<1.0.0 and >2.0.0"), strange_interval_error );
    BOOST_CHECK_THROW( Interval("<1.0.0-alpha and >1.0.0"), strange_interval_error );
}

// TODO: regex, Version constructor, get_version(), get_full(), Version exceptions
