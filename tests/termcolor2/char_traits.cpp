#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/char_traits.hpp>

#if TERMCOLOR2_STD_VER > 11
#  define TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( X ) static_assert( X, #X )
#else
#  define TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( X ) BOOST_CHECK( X )
#endif

template <typename CharT>
TERMCOLOR2_CXX14_CONSTEXPR CharT assign() {
    CharT c1 = 'a';
    termcolor2::char_traits<CharT>::assign(c1, 'b');
    return c1;
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_assign_test )
{
    TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( assign<char>() == 'b' );
    TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( assign<wchar_t>() == 'b' );
    TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( assign<char16_t>() == 'b' );
    TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( assign<char32_t>() == 'b' );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_eq_test )
{
    static_assert( termcolor2::char_traits<char>::eq('f', 'f'), "" );
    static_assert( !termcolor2::char_traits<char>::eq('f', 'd'), "" );

    static_assert( termcolor2::char_traits<wchar_t>::eq('f', 'f'), "" );
    static_assert( !termcolor2::char_traits<wchar_t>::eq('f', 'd'), "" );

    static_assert( termcolor2::char_traits<char16_t>::eq('f', 'f'), "" );
    static_assert( !termcolor2::char_traits<char16_t>::eq('f', 'd'), "" );

    static_assert( termcolor2::char_traits<char32_t>::eq('f', 'f'), "" );
    static_assert( !termcolor2::char_traits<char32_t>::eq('f', 'd'), "" );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_lt_test )
{
    static_assert( termcolor2::char_traits<char>::lt('a', 'z'), "" );
    static_assert( !termcolor2::char_traits<char>::lt('z', 'a'), "" );

    static_assert( termcolor2::char_traits<wchar_t>::lt('a', 'z'), "" );
    static_assert( !termcolor2::char_traits<wchar_t>::lt('z', 'a'), "" );

    static_assert( termcolor2::char_traits<char16_t>::lt('a', 'z'), "" );
    static_assert( !termcolor2::char_traits<char16_t>::lt('z', 'a'), "" );

    static_assert( termcolor2::char_traits<char32_t>::lt('a', 'z'), "" );
    static_assert( !termcolor2::char_traits<char32_t>::lt('z', 'a'), "" );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_compare_test )
{
    static_assert( termcolor2::char_traits<char>::compare("foo", "foo", 3) == 0, "" );
    static_assert( termcolor2::char_traits<char>::compare("aoo", "foo", 3) == -1, "" );
    static_assert( termcolor2::char_traits<char>::compare("foo", "aoo", 3) == 1, "" );
    static_assert( termcolor2::char_traits<char>::compare("foa", "foz", 3) == -1, "" );
    static_assert( termcolor2::char_traits<char>::compare("foz", "foa", 3) == 1, "" );
    static_assert( termcolor2::char_traits<char>::compare("foo", "foobar", 3) == 0, "" );

    static_assert( termcolor2::char_traits<wchar_t>::compare(L"foo", L"foo", 3) == 0, "" );
    static_assert( termcolor2::char_traits<wchar_t>::compare(L"aoo", L"foo", 3) == -1, "" );
    static_assert( termcolor2::char_traits<wchar_t>::compare(L"foo", L"aoo", 3) == 1, "" );
    static_assert( termcolor2::char_traits<wchar_t>::compare(L"foa", L"foz", 3) == -1, "" );
    static_assert( termcolor2::char_traits<wchar_t>::compare(L"foz", L"foa", 3) == 1, "" );
    static_assert( termcolor2::char_traits<wchar_t>::compare(L"foo", L"foobar", 3) == 0, "" );

    static_assert( termcolor2::char_traits<char16_t>::compare(u"foo", u"foo", 3) == 0, "" );
    static_assert( termcolor2::char_traits<char16_t>::compare(u"aoo", u"foo", 3) == -1, "" );
    static_assert( termcolor2::char_traits<char16_t>::compare(u"foo", u"aoo", 3) == 1, "" );
    static_assert( termcolor2::char_traits<char16_t>::compare(u"foa", u"foz", 3) == -1, "" );
    static_assert( termcolor2::char_traits<char16_t>::compare(u"foz", u"foa", 3) == 1, "" );
    static_assert( termcolor2::char_traits<char16_t>::compare(u"foo", u"foobar", 3) == 0, "" );

    static_assert( termcolor2::char_traits<char32_t>::compare(U"foo", U"foo", 3) == 0, "" );
    static_assert( termcolor2::char_traits<char32_t>::compare(U"aoo", U"foo", 3) == -1, "" );
    static_assert( termcolor2::char_traits<char32_t>::compare(U"foo", U"aoo", 3) == 1, "" );
    static_assert( termcolor2::char_traits<char32_t>::compare(U"foa", U"foz", 3) == -1, "" );
    static_assert( termcolor2::char_traits<char32_t>::compare(U"foz", U"foa", 3) == 1, "" );
    static_assert( termcolor2::char_traits<char32_t>::compare(U"foo", U"foobar", 3) == 0, "" );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_length_test )
{
    static_assert( termcolor2::char_traits<char>::length("foo") == 3, "" );
    static_assert( termcolor2::char_traits<char>::length("foobar") == 6, "" );
    static_assert( termcolor2::char_traits<char>::length("s") == 1, "" );
    static_assert( termcolor2::char_traits<char>::length("st") == 2, "" );
    static_assert( termcolor2::char_traits<char>::length("") == 0, "" );

    static_assert( termcolor2::char_traits<wchar_t>::length(L"foo") == 3, "" );
    static_assert( termcolor2::char_traits<wchar_t>::length(L"foobar") == 6, "" );
    static_assert( termcolor2::char_traits<wchar_t>::length(L"s") == 1, "" );
    static_assert( termcolor2::char_traits<wchar_t>::length(L"st") == 2, "" );
    static_assert( termcolor2::char_traits<wchar_t>::length(L"") == 0, "" );

    static_assert( termcolor2::char_traits<char16_t>::length(u"foo") == 3, "" );
    static_assert( termcolor2::char_traits<char16_t>::length(u"foobar") == 6, "" );
    static_assert( termcolor2::char_traits<char16_t>::length(u"s") == 1, "" );
    static_assert( termcolor2::char_traits<char16_t>::length(u"st") == 2, "" );
    static_assert( termcolor2::char_traits<char16_t>::length(u"") == 0, "" );

    static_assert( termcolor2::char_traits<char32_t>::length(U"foo") == 3, "" );
    static_assert( termcolor2::char_traits<char32_t>::length(U"foobar") == 6, "" );
    static_assert( termcolor2::char_traits<char32_t>::length(U"s") == 1, "" );
    static_assert( termcolor2::char_traits<char32_t>::length(U"st") == 2, "" );
    static_assert( termcolor2::char_traits<char32_t>::length(U"") == 0, "" );
}

BOOST_AUTO_TEST_CASE( termcolor2_char_traits_find_test )
{
    static_assert( *termcolor2::char_traits<char>::find("bar", 3, 'b') == 'b', "" );
    static_assert( *termcolor2::char_traits<char>::find("bar", 3, 'a') == 'a', "" );
    static_assert( *termcolor2::char_traits<char>::find("bar", 3, 'r') == 'r', "" );
    static_assert( termcolor2::char_traits<char>::find("bar", 3, 'z') == 0, "" );

    static_assert( *termcolor2::char_traits<wchar_t>::find(L"bar", 3, 'b') == 'b', "" );
    static_assert( *termcolor2::char_traits<wchar_t>::find(L"bar", 3, 'a') == 'a', "" );
    static_assert( *termcolor2::char_traits<wchar_t>::find(L"bar", 3, 'r') == 'r', "" );
    static_assert( termcolor2::char_traits<wchar_t>::find(L"bar", 3, 'z') == 0, "" );

    static_assert( *termcolor2::char_traits<char16_t>::find(u"bar", 3, 'b') == 'b', "" );
    static_assert( *termcolor2::char_traits<char16_t>::find(u"bar", 3, 'a') == 'a', "" );
    static_assert( *termcolor2::char_traits<char16_t>::find(u"bar", 3, 'r') == 'r', "" );
    static_assert( termcolor2::char_traits<char16_t>::find(u"bar", 3, 'z') == 0, "" );

    static_assert( *termcolor2::char_traits<char32_t>::find(U"bar", 3, 'b') == 'b', "" );
    static_assert( *termcolor2::char_traits<char32_t>::find(U"bar", 3, 'a') == 'a', "" );
    static_assert( *termcolor2::char_traits<char32_t>::find(U"bar", 3, 'r') == 'r', "" );
    static_assert( termcolor2::char_traits<char32_t>::find(U"bar", 3, 'z') == 0, "" );
}
