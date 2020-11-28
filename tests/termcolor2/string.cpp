#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/string.hpp>
#include <poac/util/termcolor2/alias.hpp>
#include <poac/util/termcolor2/comparison.hpp>

#if TERMCOLOR2_STD_VER > 11
#  define TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( X ) static_assert( X, #X )
#else
#  define TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11( X ) BOOST_CHECK( X )
#endif

BOOST_AUTO_TEST_CASE( termcolor2_string_test )
{
    // FIXME
//    {
//        constexpr char const* s = "foo";
//        constexpr termcolor2::string<3> str(s);
//        static_assert(str.size() == 3, "");
//        static_assert(str == s, "");
//    }
    {
#if TERMCOLOR2_STD_VER > 11
        constexpr termcolor2::string<3> str({'f', 'o', 'o'});
#else
        const termcolor2::string<3> str({'f', 'o', 'o'});
#endif
        TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11(str.size() == 3);
        TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11(str == "foo");
    }
    {
        constexpr termcolor2::string<3> str(3, 'f', 'o', 'o');
        static_assert(str.size() == 3, "");
        static_assert(str == "foo", "");
    }
}
BOOST_AUTO_TEST_CASE( termcolor2_wstring_test )
{
//    {
//        constexpr wchar_t const* s = L"foo";
//        constexpr termcolor2::wstring<3> str(s);
//        static_assert(str.size() == 3, "");
//        static_assert(str == s, "");
//    }
    {
#if TERMCOLOR2_STD_VER > 11
        constexpr termcolor2::wstring<3> str({L'f', L'o', L'o'});
#else
        const termcolor2::wstring<3> str({L'f', L'o', L'o'});
#endif
        TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11(str.size() == 3);
        TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11(str == L"foo");
    }
    {
        constexpr termcolor2::wstring<3> str(3, L'f', L'o', L'o');
        static_assert(str.size() == 3, "");
        static_assert(str == L"foo", "");
    }
}
BOOST_AUTO_TEST_CASE( termcolor2_u16string_test )
{
//    {
//        constexpr char16_t const* s = u"foo";
//        constexpr termcolor2::u16string<3> str(s);
//        static_assert(str.size() == 3, "");
//        static_assert(str == s, "");
//    }
    {
#if TERMCOLOR2_STD_VER > 11
        constexpr termcolor2::u16string<3> str({u'f', u'o', u'o'});
#else
        const termcolor2::u16string<3> str({u'f', u'o', u'o'});
#endif
        TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11(str.size() == 3);
        TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11(str == u"foo");
    }
    {
        constexpr termcolor2::u16string<3> str(3, u'f', u'o', u'o');
        static_assert(str.size() == 3, "");
        static_assert(str == u"foo", "");
    }
}
BOOST_AUTO_TEST_CASE( termcolor2_u32string_test )
{
//    {
//        constexpr char32_t const* s = U"foo";
//        constexpr termcolor2::u32string<3> str(s);
//        static_assert(str.size() == 3, "");
//        static_assert(str == s, "");
//    }
    {
#if TERMCOLOR2_STD_VER > 11
        constexpr termcolor2::u32string<3> str({U'f', U'o', U'o'});
#else
        const termcolor2::u32string<3> str({U'f', U'o', U'o'});
#endif
        TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11(str.size() == 3);
        TERMCOLOR2_STATIC_ASSERT_AFTER_CXX11(str == U"foo");
    }
    {
        constexpr termcolor2::u32string<3> str(3, U'f', U'o', U'o');
        static_assert(str.size() == 3, "");
        static_assert(str == U"foo", "");
    }
}

// empty
BOOST_AUTO_TEST_CASE( termcolor2_string_empty_test )
{
//    constexpr char const* s = "";
//    constexpr termcolor2::string<3> str(s);
//    static_assert(str.empty(), "");
}
BOOST_AUTO_TEST_CASE( termcolor2_wstring_empty_test )
{
//    constexpr wchar_t const* s = L"";
//    constexpr termcolor2::wstring<3> str(s);
//    static_assert(str.empty(), "");
}
BOOST_AUTO_TEST_CASE( termcolor2_u16string_empty_test )
{
//    constexpr char16_t const* s = u"";
//    constexpr termcolor2::u16string<3> str(s);
//    static_assert(str.empty(), "");
}
BOOST_AUTO_TEST_CASE( termcolor2_u32string_empty_test )
{
//    constexpr char32_t const* s = U"";
//    constexpr termcolor2::u32string<3> str(s);
//    static_assert(str.empty(), "");
}

// operator[]
BOOST_AUTO_TEST_CASE( termcolor2_string_operator_ref_test )
{
//    constexpr char const* s = "foo";
//    constexpr termcolor2::string<3> str(s);
//    static_assert(str[0] == 'f', "");
//    static_assert(str[1] == 'o', "");
//    static_assert(str[2] == 'o', "");
}
BOOST_AUTO_TEST_CASE( termcolor2_wstring_operator_ref_test )
{
//    constexpr wchar_t const* s = L"foo";
//    constexpr termcolor2::wstring<3> str(s);
//    static_assert(str[0] == L'f', "");
//    static_assert(str[1] == L'o', "");
//    static_assert(str[2] == L'o', "");
}
BOOST_AUTO_TEST_CASE( termcolor2_u16string_operator_ref_test )
{
//    constexpr char16_t const* s = u"foo";
//    constexpr termcolor2::u16string<3> str(s);
//    static_assert(str[0] == u'f', "");
//    static_assert(str[1] == u'o', "");
//    static_assert(str[2] == u'o', "");
}
BOOST_AUTO_TEST_CASE( termcolor2_u32string_operator_ref_test )
{
//    constexpr char32_t const* s = U"foo";
//    constexpr termcolor2::u32string<3> str(s);
//    static_assert(str[0] == U'f', "");
//    static_assert(str[1] == U'o', "");
//    static_assert(str[2] == U'o', "");
}
