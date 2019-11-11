#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/min.hpp>

BOOST_AUTO_TEST_CASE( termcolor2_min_test )
{
    static_assert(termcolor2::min(1, 2) == 1, "");
    static_assert(termcolor2::min(2, 1) == 1, "");
    static_assert(termcolor2::min('b', 'z') == 'b', "");
    static_assert(termcolor2::min('z', 'b') == 'b', "");
}
