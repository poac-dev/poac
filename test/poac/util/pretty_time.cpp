#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <exception>

#include <poac/util/pretty_time.hpp>


// std::string to(const std::string& s)
BOOST_AUTO_TEST_CASE( poac_util_pretty_time_test1 )
{
    using namespace poac::util;

    BOOST_TEST( pretty_time::to("0.5") == "0.5s" );
    BOOST_TEST( pretty_time::to("1.1") == "1s" );
    BOOST_TEST( pretty_time::to("60") == "1m 0s" );
    BOOST_TEST( pretty_time::to("60.1") == "1m 0s" );
    BOOST_TEST( pretty_time::to("60.5") == "1m 0s" );
    BOOST_TEST( pretty_time::to("70") == "1m 10s" );
    BOOST_TEST( pretty_time::to("3600") == "1h 0s" );
    BOOST_TEST( pretty_time::to("3670") == "1h 1m 10s" );
    BOOST_TEST( pretty_time::to("86400") == "1d 0s" );
    BOOST_TEST( pretty_time::to("86470") == "1d 1m 10s" );
    BOOST_TEST( pretty_time::to("90070") == "1d 1h 1m 10s" );
    BOOST_TEST( pretty_time::to("900q70") == "15m 0s" ); // 900
    BOOST_CHECK_THROW( pretty_time::to("r90"), std::invalid_argument );
}
