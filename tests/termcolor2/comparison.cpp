#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/comparison.hpp>

// operator==
// operator!=
BOOST_AUTO_TEST_CASE( termcolor2_char_traits_eq_test )
{
    static_assert(termcolor2::make_string("foo") == termcolor2::make_string("foo"), "");
    static_assert(termcolor2::make_string("foo") == "foo", "");
    static_assert("foo" == termcolor2::make_string("foo"), "");
    static_assert(termcolor2::make_string("foo") != termcolor2::make_string("bar"), "");
    static_assert(termcolor2::make_string("foo") != "fooo", "");
    static_assert("foo " != termcolor2::make_string("foo"), "");
}
