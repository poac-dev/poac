#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/string.hpp>
#include <poac/util/termcolor2/comparison.hpp>

BOOST_AUTO_TEST_CASE( termcolor2_string_test )
{
    {
        constexpr char const* s = "foo";
        constexpr termcolor2::basic_string<char, 3> str(s);
        static_assert(str.size() == 3);
        static_assert(str == s);
    }
    {
        constexpr termcolor2::basic_string<char, 3> str({'f', 'o', 'o'});
        static_assert(str.size() == 3);
        static_assert(str == "foo");
    }
    {
        constexpr termcolor2::basic_string<char, 3> str(3, 'f', 'o', 'o');
        static_assert(str.size() == 3);
        static_assert(str == "foo");
    }
}

// empty
BOOST_AUTO_TEST_CASE( termcolor2_string_empty_test )
{
    constexpr char const* s = "";
    constexpr termcolor2::basic_string<char, 3> str(s);
    static_assert(str.empty());
}

// operator[]
BOOST_AUTO_TEST_CASE( termcolor2_string_operator_ref_test )
{
    constexpr char const* s = "foo";
    constexpr termcolor2::basic_string<char, 3> str(s);
    static_assert(str[0] == 'f');
    static_assert(str[1] == 'o');
    static_assert(str[2] == 'o');
}
