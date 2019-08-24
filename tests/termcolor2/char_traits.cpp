#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/char_traits.hpp>

#if TERMCOLOR2_STD_VER > 11
#  define TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( X ) static_assert( X, #X )
#else
#  define TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( X ) BOOST_CHECK( X )
#endif

TERMCOLOR2_CXX14_CONSTEXPR char assign() {
    char c1 = 'a';
    termcolor2::char_traits<char>::assign(c1, 'b');
    return c1;
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_assign_test )
{
    TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( assign() == 'b' );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_eq_test )
{
    static_assert( termcolor2::char_traits<char>::eq('f', 'f'), "" );
    static_assert( !termcolor2::char_traits<char>::eq('f', 'd'), "" );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_lt_test )
{
    static_assert( termcolor2::char_traits<char>::lt('a', 'z'), "" );
    static_assert( !termcolor2::char_traits<char>::lt('z', 'a'), "" );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_compare_test )
{
    static_assert( termcolor2::char_traits<char>::compare("foo", "foo", 3) == 0, "" );
    static_assert( termcolor2::char_traits<char>::compare("aoo", "foo", 3) == -1, "" );
    static_assert( termcolor2::char_traits<char>::compare("foo", "aoo", 3) == 1, "" );
    static_assert( termcolor2::char_traits<char>::compare("foa", "foz", 3) == -1, "" );
    static_assert( termcolor2::char_traits<char>::compare("foz", "foa", 3) == 1, "" );
    static_assert( termcolor2::char_traits<char>::compare("foo", "foobar", 3) == 0, "" );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_length_test )
{
    static_assert( termcolor2::char_traits<char>::length("foo") == 3, "" );
    static_assert( termcolor2::char_traits<char>::length("foobar") == 6, "" );
    static_assert( termcolor2::char_traits<char>::length("s") == 1, "" );
    static_assert( termcolor2::char_traits<char>::length("st") == 2, "" );
    static_assert( termcolor2::char_traits<char>::length("") == 0, "" );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_find_test )
{
    static_assert( *termcolor2::char_traits<char>::find("bar", 3, 'b') == 'b', "" );
    static_assert( *termcolor2::char_traits<char>::find("bar", 3, 'a') == 'a', "" );
    static_assert( *termcolor2::char_traits<char>::find("bar", 3, 'r') == 'r', "" );
    static_assert( termcolor2::char_traits<char>::find("bar", 3, 'z') == 0, "" );
}
