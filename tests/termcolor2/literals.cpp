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
}
