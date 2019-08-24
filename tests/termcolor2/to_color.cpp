#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/util/termcolor2/to_color.hpp>
#include <poac/util/termcolor2/comparison.hpp>
#include <poac/util/termcolor2/concat.hpp>

BOOST_AUTO_TEST_CASE( termcolor2_to_color_foreground_manipulators_test )
{
    BOOST_CHECK((termcolor2::to_gray<char, 3, 'f', 'o', 'o'>()) == termcolor2::gray + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_gray("foo") == termcolor2::gray + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_gray("foo", 3) == (termcolor2::gray + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_gray(std::string("foo")) == (termcolor2::gray + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_red<char, 3, 'f', 'o', 'o'>()) == termcolor2::red + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_red("foo") == termcolor2::red + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_red("foo", 3) == (termcolor2::red + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_red(std::string("foo")) == (termcolor2::red + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_green<char, 3, 'f', 'o', 'o'>()) == termcolor2::green + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_green("foo") == termcolor2::green + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_green("foo", 3) == (termcolor2::green + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_green(std::string("foo")) == (termcolor2::green + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_yellow<char, 3, 'f', 'o', 'o'>()) == termcolor2::yellow + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_yellow("foo") == termcolor2::yellow + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_yellow("foo", 3) == (termcolor2::yellow + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_yellow(std::string("foo")) == (termcolor2::yellow + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_blue<char, 3, 'f', 'o', 'o'>()) == termcolor2::blue + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_blue("foo") == termcolor2::blue + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_blue("foo", 3) == (termcolor2::blue + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_blue(std::string("foo")) == (termcolor2::blue + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_magenta<char, 3, 'f', 'o', 'o'>()) == termcolor2::magenta + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_magenta("foo") == termcolor2::magenta + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_magenta("foo", 3) == (termcolor2::magenta + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_magenta(std::string("foo")) == (termcolor2::magenta + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_cyan<char, 3, 'f', 'o', 'o'>()) == termcolor2::cyan + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_cyan("foo") == termcolor2::cyan + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_cyan("foo", 3) == (termcolor2::cyan + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_cyan(std::string("foo")) == (termcolor2::cyan + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_white<char, 3, 'f', 'o', 'o'>()) == termcolor2::white + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_white("foo") == termcolor2::white + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_white("foo", 3) == (termcolor2::white + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_white(std::string("foo")) == (termcolor2::white + "foo" + termcolor2::reset).to_string());
}

BOOST_AUTO_TEST_CASE( termcolor2_to_color_background_manipulators_test )
{
    BOOST_CHECK((termcolor2::to_on_gray<char, 3, 'f', 'o', 'o'>()) == termcolor2::on_gray + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_gray("foo") == termcolor2::on_gray + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_gray("foo", 3) == (termcolor2::on_gray + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_on_gray(std::string("foo")) == (termcolor2::on_gray + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_on_red<char, 3, 'f', 'o', 'o'>()) == termcolor2::on_red + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_red("foo") == termcolor2::on_red + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_red("foo", 3) == (termcolor2::on_red + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_on_red(std::string("foo")) == (termcolor2::on_red + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_on_green<char, 3, 'f', 'o', 'o'>()) == termcolor2::on_green + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_green("foo") == termcolor2::on_green + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_green("foo", 3) == (termcolor2::on_green + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_on_green(std::string("foo")) == (termcolor2::on_green + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_on_yellow<char, 3, 'f', 'o', 'o'>()) == termcolor2::on_yellow + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_yellow("foo") == termcolor2::on_yellow + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_yellow("foo", 3) == (termcolor2::on_yellow + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_on_yellow(std::string("foo")) == (termcolor2::on_yellow + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_on_blue<char, 3, 'f', 'o', 'o'>()) == termcolor2::on_blue + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_blue("foo") == termcolor2::on_blue + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_blue("foo", 3) == (termcolor2::on_blue + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_on_blue(std::string("foo")) == (termcolor2::on_blue + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_on_magenta<char, 3, 'f', 'o', 'o'>()) == termcolor2::on_magenta + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_magenta("foo") == termcolor2::on_magenta + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_magenta("foo", 3) == (termcolor2::on_magenta + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_on_magenta(std::string("foo")) == (termcolor2::on_magenta + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_on_cyan<char, 3, 'f', 'o', 'o'>()) == termcolor2::on_cyan + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_cyan("foo") == termcolor2::on_cyan + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_cyan("foo", 3) == (termcolor2::on_cyan + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_on_cyan(std::string("foo")) == (termcolor2::on_cyan + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_on_white<char, 3, 'f', 'o', 'o'>()) == termcolor2::on_white + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_white("foo") == termcolor2::on_white + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_on_white("foo", 3) == (termcolor2::on_white + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_on_white(std::string("foo")) == (termcolor2::on_white + "foo" + termcolor2::reset).to_string());
}

BOOST_AUTO_TEST_CASE( termcolor2_to_color_attribute_manipulators_test )
{
    BOOST_CHECK((termcolor2::to_bold<char, 3, 'f', 'o', 'o'>()) == termcolor2::bold + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_bold("foo") == termcolor2::bold + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_bold("foo", 3) == (termcolor2::bold + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_bold(std::string("foo")) == (termcolor2::bold + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_dark<char, 3, 'f', 'o', 'o'>()) == termcolor2::dark + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_dark("foo") == termcolor2::dark + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_dark("foo", 3) == (termcolor2::dark + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_dark(std::string("foo")) == (termcolor2::dark + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_underline<char, 3, 'f', 'o', 'o'>()) == termcolor2::underline + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_underline("foo") == termcolor2::underline + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_underline("foo", 3) == (termcolor2::underline + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_underline(std::string("foo")) == (termcolor2::underline + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_blink<char, 3, 'f', 'o', 'o'>()) == termcolor2::blink + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_blink("foo") == termcolor2::blink + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_blink("foo", 3) == (termcolor2::blink + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_blink(std::string("foo")) == (termcolor2::blink + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_reverse<char, 3, 'f', 'o', 'o'>()) == termcolor2::reverse + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_reverse("foo") == termcolor2::reverse + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_reverse("foo", 3) == (termcolor2::reverse + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_reverse(std::string("foo")) == (termcolor2::reverse + "foo" + termcolor2::reset).to_string());

    BOOST_CHECK((termcolor2::to_concealed<char, 3, 'f', 'o', 'o'>()) == termcolor2::concealed + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_concealed("foo") == termcolor2::concealed + "foo" + termcolor2::reset); // FIXME: static_assert
    BOOST_CHECK(termcolor2::to_concealed("foo", 3) == (termcolor2::concealed + "foo" + termcolor2::reset).to_string());
    BOOST_CHECK(termcolor2::to_concealed(std::string("foo")) == (termcolor2::concealed + "foo" + termcolor2::reset).to_string());
}
