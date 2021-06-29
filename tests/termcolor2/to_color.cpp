#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/to_color.hpp>

BOOST_AUTO_TEST_CASE( termcolor2_to_color_foreground_manipulators_test )
{
    // FIXME
//    static_assert((termcolor2::to_gray<char, 'f', 'o', 'o'>()) == termcolor2::gray + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_gray("foo") == termcolor2::gray + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_gray("foo") == termcolor2::gray + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_gray(std::string("foo")) == termcolor2::gray + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_red<char, 'f', 'o', 'o'>()) == termcolor2::red + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_red("foo") == termcolor2::red + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_red("foo") == termcolor2::red + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_red(std::string("foo")) == termcolor2::red + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_green<char, 'f', 'o', 'o'>()) == termcolor2::green + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_green("foo") == termcolor2::green + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_green("foo") == termcolor2::green + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_green(std::string("foo")) == termcolor2::green + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_yellow<char, 'f', 'o', 'o'>()) == termcolor2::yellow + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_yellow("foo") == termcolor2::yellow + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_yellow("foo") == termcolor2::yellow + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_yellow(std::string("foo")) == termcolor2::yellow + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_blue<char, 'f', 'o', 'o'>()) == termcolor2::blue + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_blue("foo") == termcolor2::blue + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_blue("foo") == termcolor2::blue + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_blue(std::string("foo")) == termcolor2::blue + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_magenta<char, 'f', 'o', 'o'>()) == termcolor2::magenta + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_magenta("foo") == termcolor2::magenta + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_magenta("foo") == termcolor2::magenta + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_magenta(std::string("foo")) == termcolor2::magenta + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_cyan<char, 'f', 'o', 'o'>()) == termcolor2::cyan + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_cyan("foo") == termcolor2::cyan + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_cyan("foo") == termcolor2::cyan + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_cyan(std::string("foo")) == termcolor2::cyan + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_white<char, 'f', 'o', 'o'>()) == termcolor2::white + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_white("foo") == termcolor2::white + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_white("foo") == termcolor2::white + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_white(std::string("foo")) == termcolor2::white + "foo" + termcolor2::reset);


//    static_assert((termcolor2::to_gray<wchar_t, 'f', 'o', 'o'>()) == termcolor2::gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_gray(L"foo") == termcolor2::gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_gray(L"foo") == termcolor2::gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_gray(std::wstring(L"foo")) == termcolor2::gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_red<wchar_t, 'f', 'o', 'o'>()) == termcolor2::red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_red(L"foo") == termcolor2::red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_red(L"foo") == termcolor2::red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_red(std::wstring(L"foo")) == termcolor2::red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_green<wchar_t, 'f', 'o', 'o'>()) == termcolor2::green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_green(L"foo") == termcolor2::green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_green(L"foo") == termcolor2::green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_green(std::wstring(L"foo")) == termcolor2::green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_yellow<wchar_t, 'f', 'o', 'o'>()) == termcolor2::yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_yellow(L"foo") == termcolor2::yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_yellow(L"foo") == termcolor2::yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_yellow(std::wstring(L"foo")) == termcolor2::yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_blue<wchar_t, 'f', 'o', 'o'>()) == termcolor2::blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_blue(L"foo") == termcolor2::blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_blue(L"foo") == termcolor2::blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_blue(std::wstring(L"foo")) == termcolor2::blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_magenta<wchar_t, 'f', 'o', 'o'>()) == termcolor2::magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_magenta(L"foo") == termcolor2::magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_magenta(L"foo") == termcolor2::magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_magenta(std::wstring(L"foo")) == termcolor2::magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_cyan<wchar_t, 'f', 'o', 'o'>()) == termcolor2::cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_cyan(L"foo") == termcolor2::cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_cyan(L"foo") == termcolor2::cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_cyan(std::wstring(L"foo")) == termcolor2::cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_white<wchar_t, 'f', 'o', 'o'>()) == termcolor2::white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_white(L"foo") == termcolor2::white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_white(L"foo") == termcolor2::white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_white(std::wstring(L"foo")) == termcolor2::white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());


//    static_assert((termcolor2::to_gray<char16_t, 'f', 'o', 'o'>()) == termcolor2::gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_gray(u"foo") == termcolor2::gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_gray(u"foo") == termcolor2::gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_gray(std::u16string(u"foo")) == termcolor2::gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_red<char16_t, 'f', 'o', 'o'>()) == termcolor2::red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_red(u"foo") == termcolor2::red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_red(u"foo") == termcolor2::red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_red(std::u16string(u"foo")) == termcolor2::red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_green<char16_t, 'f', 'o', 'o'>()) == termcolor2::green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_green(u"foo") == termcolor2::green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_green(u"foo") == termcolor2::green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_green(std::u16string(u"foo")) == termcolor2::green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_yellow<char16_t, 'f', 'o', 'o'>()) == termcolor2::yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_yellow(u"foo") == termcolor2::yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_yellow(u"foo") == termcolor2::yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_yellow(std::u16string(u"foo")) == termcolor2::yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_blue<char16_t, 'f', 'o', 'o'>()) == termcolor2::blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_blue(u"foo") == termcolor2::blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_blue(u"foo") == termcolor2::blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_blue(std::u16string(u"foo")) == termcolor2::blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_magenta<char16_t, 'f', 'o', 'o'>()) == termcolor2::magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_magenta(u"foo") == termcolor2::magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_magenta(u"foo") == termcolor2::magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_magenta(std::u16string(u"foo")) == termcolor2::magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_cyan<char16_t, 'f', 'o', 'o'>()) == termcolor2::cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_cyan(u"foo") == termcolor2::cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_cyan(u"foo") == termcolor2::cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_cyan(std::u16string(u"foo")) == termcolor2::cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_white<char16_t, 'f', 'o', 'o'>()) == termcolor2::white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_white(u"foo") == termcolor2::white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_white(u"foo") == termcolor2::white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_white(std::u16string(u"foo")) == termcolor2::white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());


//    static_assert((termcolor2::to_gray<char32_t, 'f', 'o', 'o'>()) == termcolor2::gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_gray(U"foo") == termcolor2::gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_gray(U"foo") == termcolor2::gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_gray(std::u32string(U"foo")) == termcolor2::gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_red<char32_t, 'f', 'o', 'o'>()) == termcolor2::red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_red(U"foo") == termcolor2::red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_red(U"foo") == termcolor2::red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_red(std::u32string(U"foo")) == termcolor2::red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_green<char32_t, 'f', 'o', 'o'>()) == termcolor2::green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_green(U"foo") == termcolor2::green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_green(U"foo") == termcolor2::green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_green(std::u32string(U"foo")) == termcolor2::green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_yellow<char32_t, 'f', 'o', 'o'>()) == termcolor2::yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_yellow(U"foo") == termcolor2::yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_yellow(U"foo") == termcolor2::yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_yellow(std::u32string(U"foo")) == termcolor2::yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_blue<char32_t, 'f', 'o', 'o'>()) == termcolor2::blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_blue(U"foo") == termcolor2::blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_blue(U"foo") == termcolor2::blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_blue(std::u32string(U"foo")) == termcolor2::blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_magenta<char32_t, 'f', 'o', 'o'>()) == termcolor2::magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_magenta(U"foo") == termcolor2::magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_magenta(U"foo") == termcolor2::magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_magenta(std::u32string(U"foo")) == termcolor2::magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_cyan<char32_t, 'f', 'o', 'o'>()) == termcolor2::cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_cyan(U"foo") == termcolor2::cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_cyan(U"foo") == termcolor2::cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_cyan(std::u32string(U"foo")) == termcolor2::cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_white<char32_t, 'f', 'o', 'o'>()) == termcolor2::white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_white(U"foo") == termcolor2::white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_white(U"foo") == termcolor2::white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_white(std::u32string(U"foo")) == termcolor2::white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
}

BOOST_AUTO_TEST_CASE( termcolor2_to_color_background_manipulators_test )
{
//    static_assert((termcolor2::to_on_gray<char, 'f', 'o', 'o'>()) == termcolor2::on_gray + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_on_gray("foo") == termcolor2::on_gray + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_on_gray("foo") == termcolor2::on_gray + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_on_gray(std::string("foo")) == termcolor2::on_gray + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_on_red<char, 'f', 'o', 'o'>()) == termcolor2::on_red + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_on_red("foo") == termcolor2::on_red + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_on_red("foo") == termcolor2::on_red + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_on_red(std::string("foo")) == termcolor2::on_red + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_on_green<char, 'f', 'o', 'o'>()) == termcolor2::on_green + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_on_green("foo") == termcolor2::on_green + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_on_green("foo") == termcolor2::on_green + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_on_green(std::string("foo")) == termcolor2::on_green + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_on_yellow<char, 'f', 'o', 'o'>()) == termcolor2::on_yellow + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_on_yellow("foo") == termcolor2::on_yellow + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_on_yellow("foo") == termcolor2::on_yellow + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_on_yellow(std::string("foo")) == termcolor2::on_yellow + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_on_blue<char, 'f', 'o', 'o'>()) == termcolor2::on_blue + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_on_blue("foo") == termcolor2::on_blue + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_on_blue("foo") == termcolor2::on_blue + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_on_blue(std::string("foo")) == termcolor2::on_blue + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_on_magenta<char, 'f', 'o', 'o'>()) == termcolor2::on_magenta + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_on_magenta("foo") == termcolor2::on_magenta + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_on_magenta("foo") == termcolor2::on_magenta + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_on_magenta(std::string("foo")) == termcolor2::on_magenta + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_on_cyan<char, 'f', 'o', 'o'>()) == termcolor2::on_cyan + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_on_cyan("foo") == termcolor2::on_cyan + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_on_cyan("foo") == termcolor2::on_cyan + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_on_cyan(std::string("foo")) == termcolor2::on_cyan + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_on_white<char, 'f', 'o', 'o'>()) == termcolor2::on_white + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_on_white("foo") == termcolor2::on_white + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_on_white("foo") == termcolor2::on_white + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_on_white(std::string("foo")) == termcolor2::on_white + "foo" + termcolor2::reset);


//    static_assert((termcolor2::to_on_gray<wchar_t, 'f', 'o', 'o'>()) == termcolor2::on_gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_on_gray(L"foo") == termcolor2::on_gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_on_gray(L"foo") == termcolor2::on_gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_on_gray(std::wstring(L"foo")) == termcolor2::on_gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_on_red<wchar_t, 'f', 'o', 'o'>()) == termcolor2::on_red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_on_red(L"foo") == termcolor2::on_red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_on_red(L"foo") == termcolor2::on_red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_on_red(std::wstring(L"foo")) == termcolor2::on_red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_on_green<wchar_t, 'f', 'o', 'o'>()) == termcolor2::on_green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_on_green(L"foo") == termcolor2::on_green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_on_green(L"foo") == termcolor2::on_green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_on_green(std::wstring(L"foo")) == termcolor2::on_green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_on_yellow<wchar_t, 'f', 'o', 'o'>()) == termcolor2::on_yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_on_yellow(L"foo") == termcolor2::on_yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_on_yellow(L"foo") == termcolor2::on_yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_on_yellow(std::wstring(L"foo")) == termcolor2::on_yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_on_blue<wchar_t, 'f', 'o', 'o'>()) == termcolor2::on_blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_on_blue(L"foo") == termcolor2::on_blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_on_blue(L"foo") == termcolor2::on_blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_on_blue(std::wstring(L"foo")) == termcolor2::on_blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_on_magenta<wchar_t, 'f', 'o', 'o'>()) == termcolor2::on_magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_on_magenta(L"foo") == termcolor2::on_magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_on_magenta(L"foo") == termcolor2::on_magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_on_magenta(std::wstring(L"foo")) == termcolor2::on_magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_on_cyan<wchar_t, 'f', 'o', 'o'>()) == termcolor2::on_cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_on_cyan(L"foo") == termcolor2::on_cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_on_cyan(L"foo") == termcolor2::on_cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_on_cyan(std::wstring(L"foo")) == termcolor2::on_cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_on_white<wchar_t, 'f', 'o', 'o'>()) == termcolor2::on_white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_on_white(L"foo") == termcolor2::on_white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_on_white(L"foo") == termcolor2::on_white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_on_white(std::wstring(L"foo")) == termcolor2::on_white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());


//    static_assert((termcolor2::to_on_gray<char16_t, 'f', 'o', 'o'>()) == termcolor2::on_gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_on_gray(u"foo") == termcolor2::on_gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_on_gray(u"foo") == termcolor2::on_gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_on_gray(std::u16string(u"foo")) == termcolor2::on_gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_on_red<char16_t, 'f', 'o', 'o'>()) == termcolor2::on_red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_on_red(u"foo") == termcolor2::on_red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_on_red(u"foo") == termcolor2::on_red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_on_red(std::u16string(u"foo")) == termcolor2::on_red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_on_green<char16_t, 'f', 'o', 'o'>()) == termcolor2::on_green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_on_green(u"foo") == termcolor2::on_green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_on_green(u"foo") == termcolor2::on_green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_on_green(std::u16string(u"foo")) == termcolor2::on_green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_on_yellow<char16_t, 'f', 'o', 'o'>()) == termcolor2::on_yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_on_yellow(u"foo") == termcolor2::on_yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_on_yellow(u"foo") == termcolor2::on_yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_on_yellow(std::u16string(u"foo")) == termcolor2::on_yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_on_blue<char16_t, 'f', 'o', 'o'>()) == termcolor2::on_blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_on_blue(u"foo") == termcolor2::on_blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_on_blue(u"foo") == termcolor2::on_blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_on_blue(std::u16string(u"foo")) == termcolor2::on_blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_on_magenta<char16_t, 'f', 'o', 'o'>()) == termcolor2::on_magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_on_magenta(u"foo") == termcolor2::on_magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_on_magenta(u"foo") == termcolor2::on_magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_on_magenta(std::u16string(u"foo")) == termcolor2::on_magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_on_cyan<char16_t, 'f', 'o', 'o'>()) == termcolor2::on_cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_on_cyan(u"foo") == termcolor2::on_cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_on_cyan(u"foo") == termcolor2::on_cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_on_cyan(std::u16string(u"foo")) == termcolor2::on_cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_on_white<char16_t, 'f', 'o', 'o'>()) == termcolor2::on_white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_on_white(u"foo") == termcolor2::on_white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_on_white(u"foo") == termcolor2::on_white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_on_white(std::u16string(u"foo")) == termcolor2::on_white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());


//    static_assert((termcolor2::to_on_gray<char32_t, 'f', 'o', 'o'>()) == termcolor2::on_gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_on_gray(U"foo") == termcolor2::on_gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_on_gray(U"foo") == termcolor2::on_gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_on_gray(std::u32string(U"foo")) == termcolor2::on_gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_on_red<char32_t, 'f', 'o', 'o'>()) == termcolor2::on_red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_on_red(U"foo") == termcolor2::on_red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_on_red(U"foo") == termcolor2::on_red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_on_red(std::u32string(U"foo")) == termcolor2::on_red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_on_green<char32_t, 'f', 'o', 'o'>()) == termcolor2::on_green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_on_green(U"foo") == termcolor2::on_green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_on_green(U"foo") == termcolor2::on_green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_on_green(std::u32string(U"foo")) == termcolor2::on_green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_on_yellow<char32_t, 'f', 'o', 'o'>()) == termcolor2::on_yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_on_yellow(U"foo") == termcolor2::on_yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_on_yellow(U"foo") == termcolor2::on_yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_on_yellow(std::u32string(U"foo")) == termcolor2::on_yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_on_blue<char32_t, 'f', 'o', 'o'>()) == termcolor2::on_blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_on_blue(U"foo") == termcolor2::on_blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_on_blue(U"foo") == termcolor2::on_blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_on_blue(std::u32string(U"foo")) == termcolor2::on_blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_on_magenta<char32_t, 'f', 'o', 'o'>()) == termcolor2::on_magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_on_magenta(U"foo") == termcolor2::on_magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_on_magenta(U"foo") == termcolor2::on_magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_on_magenta(std::u32string(U"foo")) == termcolor2::on_magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_on_cyan<char32_t, 'f', 'o', 'o'>()) == termcolor2::on_cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_on_cyan(U"foo") == termcolor2::on_cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_on_cyan(U"foo") == termcolor2::on_cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_on_cyan(std::u32string(U"foo")) == termcolor2::on_cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_on_white<char32_t, 'f', 'o', 'o'>()) == termcolor2::on_white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_on_white(U"foo") == termcolor2::on_white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_on_white(U"foo") == termcolor2::on_white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_on_white(std::u32string(U"foo")) == termcolor2::on_white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
}

BOOST_AUTO_TEST_CASE( termcolor2_to_color_attribute_manipulators_test )
{
//    static_assert((termcolor2::to_bold<char, 'f', 'o', 'o'>()) == termcolor2::bold + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_bold("foo") == termcolor2::bold + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_bold("foo") == termcolor2::bold + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_bold(std::string("foo")) == termcolor2::bold + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_dark<char, 'f', 'o', 'o'>()) == termcolor2::dark + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_dark("foo") == termcolor2::dark + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_dark("foo") == termcolor2::dark + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_dark(std::string("foo")) == termcolor2::dark + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_underline<char, 'f', 'o', 'o'>()) == termcolor2::underline + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_underline("foo") == termcolor2::underline + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_underline("foo") == termcolor2::underline + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_underline(std::string("foo")) == termcolor2::underline + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_blink<char, 'f', 'o', 'o'>()) == termcolor2::blink + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_blink("foo") == termcolor2::blink + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_blink("foo") == termcolor2::blink + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_blink(std::string("foo")) == termcolor2::blink + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_reverse<char, 'f', 'o', 'o'>()) == termcolor2::reverse + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_reverse("foo") == termcolor2::reverse + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_reverse("foo") == termcolor2::reverse + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_reverse(std::string("foo")) == termcolor2::reverse + "foo" + termcolor2::reset);

//    static_assert((termcolor2::to_concealed<char, 'f', 'o', 'o'>()) == termcolor2::concealed + "foo" + termcolor2::reset, "");
//    static_assert(termcolor2::to_concealed("foo") == termcolor2::concealed + "foo" + termcolor2::reset, "");
    BOOST_CHECK(termcolor2::to_concealed("foo") == termcolor2::concealed + "foo" + termcolor2::reset);
    BOOST_CHECK(termcolor2::to_concealed(std::string("foo")) == termcolor2::concealed + "foo" + termcolor2::reset);


//    static_assert((termcolor2::to_bold<wchar_t, 'f', 'o', 'o'>()) == termcolor2::bold_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_bold(L"foo") == termcolor2::bold_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_bold(L"foo") == termcolor2::bold_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_bold(std::wstring(L"foo")) == termcolor2::bold_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_dark<wchar_t, 'f', 'o', 'o'>()) == termcolor2::dark_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_dark(L"foo") == termcolor2::dark_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_dark(L"foo") == termcolor2::dark_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_dark(std::wstring(L"foo")) == termcolor2::dark_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_underline<wchar_t, 'f', 'o', 'o'>()) == termcolor2::underline_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_underline(L"foo") == termcolor2::underline_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_underline(L"foo") == termcolor2::underline_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_underline(std::wstring(L"foo")) == termcolor2::underline_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_blink<wchar_t, 'f', 'o', 'o'>()) == termcolor2::blink_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_blink(L"foo") == termcolor2::blink_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_blink(L"foo") == termcolor2::blink_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_blink(std::wstring(L"foo")) == termcolor2::blink_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_reverse<wchar_t, 'f', 'o', 'o'>()) == termcolor2::reverse_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_reverse(L"foo") == termcolor2::reverse_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_reverse(L"foo") == termcolor2::reverse_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_reverse(std::wstring(L"foo")) == termcolor2::reverse_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());

//    static_assert((termcolor2::to_concealed<wchar_t, 'f', 'o', 'o'>()) == termcolor2::concealed_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
//    static_assert(termcolor2::to_concealed(L"foo") == termcolor2::concealed_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>(), "");
    BOOST_CHECK(termcolor2::to_concealed(L"foo") == termcolor2::concealed_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());
    BOOST_CHECK(termcolor2::to_concealed(std::wstring(L"foo")) == termcolor2::concealed_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>());


//    static_assert((termcolor2::to_bold<char16_t, 'f', 'o', 'o'>()) == termcolor2::bold_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_bold(u"foo") == termcolor2::bold_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_bold(u"foo") == termcolor2::bold_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_bold(std::u16string(u"foo")) == termcolor2::bold_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_dark<char16_t, 'f', 'o', 'o'>()) == termcolor2::dark_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_dark(u"foo") == termcolor2::dark_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_dark(u"foo") == termcolor2::dark_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_dark(std::u16string(u"foo")) == termcolor2::dark_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_underline<char16_t, 'f', 'o', 'o'>()) == termcolor2::underline_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_underline(u"foo") == termcolor2::underline_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_underline(u"foo") == termcolor2::underline_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_underline(std::u16string(u"foo")) == termcolor2::underline_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_blink<char16_t, 'f', 'o', 'o'>()) == termcolor2::blink_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_blink(u"foo") == termcolor2::blink_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_blink(u"foo") == termcolor2::blink_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_blink(std::u16string(u"foo")) == termcolor2::blink_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_reverse<char16_t, 'f', 'o', 'o'>()) == termcolor2::reverse_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_reverse(u"foo") == termcolor2::reverse_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_reverse(u"foo") == termcolor2::reverse_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_reverse(std::u16string(u"foo")) == termcolor2::reverse_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());

//    static_assert((termcolor2::to_concealed<char16_t, 'f', 'o', 'o'>()) == termcolor2::concealed_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
//    static_assert(termcolor2::to_concealed(u"foo") == termcolor2::concealed_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>(), "");
    BOOST_CHECK(termcolor2::to_concealed(u"foo") == termcolor2::concealed_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());
    BOOST_CHECK(termcolor2::to_concealed(std::u16string(u"foo")) == termcolor2::concealed_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>());


//    static_assert((termcolor2::to_bold<char32_t, 'f', 'o', 'o'>()) == termcolor2::bold_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_bold(U"foo") == termcolor2::bold_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_bold(U"foo") == termcolor2::bold_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_bold(std::u32string(U"foo")) == termcolor2::bold_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_dark<char32_t, 'f', 'o', 'o'>()) == termcolor2::dark_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_dark(U"foo") == termcolor2::dark_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_dark(U"foo") == termcolor2::dark_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_dark(std::u32string(U"foo")) == termcolor2::dark_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_underline<char32_t, 'f', 'o', 'o'>()) == termcolor2::underline_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_underline(U"foo") == termcolor2::underline_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_underline(U"foo") == termcolor2::underline_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_underline(std::u32string(U"foo")) == termcolor2::underline_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_blink<char32_t, 'f', 'o', 'o'>()) == termcolor2::blink_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_blink(U"foo") == termcolor2::blink_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_blink(U"foo") == termcolor2::blink_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_blink(std::u32string(U"foo")) == termcolor2::blink_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_reverse<char32_t, 'f', 'o', 'o'>()) == termcolor2::reverse_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_reverse(U"foo") == termcolor2::reverse_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_reverse(U"foo") == termcolor2::reverse_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_reverse(std::u32string(U"foo")) == termcolor2::reverse_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());

//    static_assert((termcolor2::to_concealed<char32_t, 'f', 'o', 'o'>()) == termcolor2::concealed_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
//    static_assert(termcolor2::to_concealed(U"foo") == termcolor2::concealed_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>(), "");
    BOOST_CHECK(termcolor2::to_concealed(U"foo") == termcolor2::concealed_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
    BOOST_CHECK(termcolor2::to_concealed(std::u32string(U"foo")) == termcolor2::concealed_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>());
}
