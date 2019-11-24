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

    static_assert(termcolor2::make_string(L"foo") + L'b' == L"foob", "");
    static_assert(L'f' + termcolor2::make_string(L"bar") == L"fbar", "");
    static_assert(termcolor2::make_string(L"foo") + L"bar" == L"foobar", "");
    static_assert(L"foo" + termcolor2::make_string(L"bar") == L"foobar", "");
    static_assert(termcolor2::make_string(L"foo") + termcolor2::make_string(L"bar") == L"foobar", "");

    static_assert(termcolor2::make_string(u"foo") + u'b' == u"foob", "");
    static_assert(u'f' + termcolor2::make_string(u"bar") == u"fbar", "");
    static_assert(termcolor2::make_string(u"foo") + u"bar" == u"foobar", "");
    static_assert(u"foo" + termcolor2::make_string(u"bar") == u"foobar", "");
    static_assert(termcolor2::make_string(u"foo") + termcolor2::make_string(u"bar") == u"foobar", "");

    static_assert(termcolor2::make_string(U"foo") + U'b' == U"foob", "");
    static_assert(U'f' + termcolor2::make_string(U"bar") == U"fbar", "");
    static_assert(termcolor2::make_string(U"foo") + U"bar" == U"foobar", "");
    static_assert(U"foo" + termcolor2::make_string(U"bar") == U"foobar", "");
    static_assert(termcolor2::make_string(U"foo") + termcolor2::make_string(U"bar") == U"foobar", "");
}
