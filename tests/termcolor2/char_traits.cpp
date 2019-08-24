#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/char_traits.hpp>

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_compare_test )
{
    static_assert( termcolor2::char_traits<char>::compare("foo", "foo", 3) == 0 );
    static_assert( termcolor2::char_traits<char>::compare("aoo", "foo", 3) == -1 );
    static_assert( termcolor2::char_traits<char>::compare("foo", "aoo", 3) == 1 );
    static_assert( termcolor2::char_traits<char>::compare("foa", "foz", 3) == -1 );
    static_assert( termcolor2::char_traits<char>::compare("foz", "foa", 3) == 1 );
    static_assert( termcolor2::char_traits<char>::compare("foo", "foobar", 3) == 0 );
}
