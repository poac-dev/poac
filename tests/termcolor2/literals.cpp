#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/literals.hpp>
#include <poac/util/termcolor2/concat.hpp>

BOOST_AUTO_TEST_CASE( termcolor2_literals_color_literals_foreground_literals_test )
{
    using namespace termcolor2::color_literals::foreground_literals;
    BOOST_CHECK( "foo"_gray == (termcolor2::gray + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_red == (termcolor2::red + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_green == (termcolor2::green + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_yellow == (termcolor2::yellow + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_blue == (termcolor2::blue + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_magenta == (termcolor2::magenta + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_cyan == (termcolor2::cyan + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_white == (termcolor2::white + "foo" + termcolor2::reset).to_string() );

    BOOST_CHECK( L"foo"_gray == (termcolor2::gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_red == (termcolor2::red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_green == (termcolor2::green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_yellow == (termcolor2::yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_blue == (termcolor2::blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_magenta == (termcolor2::magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_cyan == (termcolor2::cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_white == (termcolor2::white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );

    BOOST_CHECK( u"foo"_gray == (termcolor2::gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_red == (termcolor2::red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_green == (termcolor2::green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_yellow == (termcolor2::yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_blue == (termcolor2::blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_magenta == (termcolor2::magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_cyan == (termcolor2::cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_white == (termcolor2::white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );

    BOOST_CHECK( U"foo"_gray == (termcolor2::gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_red == (termcolor2::red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_green == (termcolor2::green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_yellow == (termcolor2::yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_blue == (termcolor2::blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_magenta == (termcolor2::magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_cyan == (termcolor2::cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_white == (termcolor2::white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
}

BOOST_AUTO_TEST_CASE( termcolor2_literals_color_literals_background_literals_test )
{
    using namespace termcolor2::color_literals::background_literals;
    BOOST_CHECK( "foo"_on_gray == (termcolor2::on_gray + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_on_red == (termcolor2::on_red + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_on_green == (termcolor2::on_green + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_on_yellow == (termcolor2::on_yellow + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_on_blue == (termcolor2::on_blue + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_on_magenta == (termcolor2::on_magenta + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_on_cyan == (termcolor2::on_cyan + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_on_white == (termcolor2::on_white + "foo" + termcolor2::reset).to_string() );

    BOOST_CHECK( L"foo"_on_gray == (termcolor2::on_gray_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_on_red == (termcolor2::on_red_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_on_green == (termcolor2::on_green_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_on_yellow == (termcolor2::on_yellow_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_on_blue == (termcolor2::on_blue_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_on_magenta == (termcolor2::on_magenta_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_on_cyan == (termcolor2::on_cyan_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_on_white == (termcolor2::on_white_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );

    BOOST_CHECK( u"foo"_on_gray == (termcolor2::on_gray_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_on_red == (termcolor2::on_red_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_on_green == (termcolor2::on_green_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_on_yellow == (termcolor2::on_yellow_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_on_blue == (termcolor2::on_blue_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_on_magenta == (termcolor2::on_magenta_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_on_cyan == (termcolor2::on_cyan_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_on_white == (termcolor2::on_white_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );

    BOOST_CHECK( U"foo"_on_gray == (termcolor2::on_gray_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_on_red == (termcolor2::on_red_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_on_green == (termcolor2::on_green_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_on_yellow == (termcolor2::on_yellow_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_on_blue == (termcolor2::on_blue_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_on_magenta == (termcolor2::on_magenta_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_on_cyan == (termcolor2::on_cyan_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_on_white == (termcolor2::on_white_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
}

BOOST_AUTO_TEST_CASE( termcolor2_literals_color_literals_attribute_literals_test )
{
    using namespace termcolor2::color_literals::attribute_literals;
    BOOST_CHECK( "foo"_bold == (termcolor2::bold + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_dark == (termcolor2::dark + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_underline == (termcolor2::underline + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_blink == (termcolor2::blink + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_reverse == (termcolor2::reverse + "foo" + termcolor2::reset).to_string() );
    BOOST_CHECK( "foo"_concealed == (termcolor2::concealed + "foo" + termcolor2::reset).to_string() );

    BOOST_CHECK( L"foo"_bold == (termcolor2::bold_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_dark == (termcolor2::dark_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_underline == (termcolor2::underline_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_blink == (termcolor2::blink_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_reverse == (termcolor2::reverse_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );
    BOOST_CHECK( L"foo"_concealed == (termcolor2::concealed_v<wchar_t>() + L"foo" + termcolor2::reset_v<wchar_t>()).to_string() );

    BOOST_CHECK( u"foo"_bold == (termcolor2::bold_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_dark == (termcolor2::dark_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_underline == (termcolor2::underline_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_blink == (termcolor2::blink_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_reverse == (termcolor2::reverse_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );
    BOOST_CHECK( u"foo"_concealed == (termcolor2::concealed_v<char16_t>() + u"foo" + termcolor2::reset_v<char16_t>()).to_string() );

    BOOST_CHECK( U"foo"_bold == (termcolor2::bold_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_dark == (termcolor2::dark_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_underline == (termcolor2::underline_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_blink == (termcolor2::blink_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_reverse == (termcolor2::reverse_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
    BOOST_CHECK( U"foo"_concealed == (termcolor2::concealed_v<char32_t>() + U"foo" + termcolor2::reset_v<char32_t>()).to_string() );
}
