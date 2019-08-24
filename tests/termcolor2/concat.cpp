#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/concat.hpp>
#include <poac/util/termcolor2/comparison.hpp>

// operator+
BOOST_AUTO_TEST_CASE( termcolor2_oparator_plus_test )
{
    static_assert(termcolor2::make_string("foo") + 'b' == "foob", "");
    static_assert('f' + termcolor2::make_string("bar") == "fbar", "");
    static_assert(termcolor2::make_string("foo") + "bar" == "foobar", "");
    static_assert("foo" + termcolor2::make_string("bar") == "foobar", "");
    static_assert(termcolor2::make_string("foo") + termcolor2::make_string("bar") == "foobar", "");
}
