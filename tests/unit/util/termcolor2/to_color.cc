#include <boost/ut.hpp>
#include <poac/util/termcolor2/to_color.hpp>

auto
main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;
  termcolor2::set_color_mode(spdlog::color_mode::always); // for CI

  "test to_color_foreground_manipulators"_test = [] {
    expect(eq(
        termcolor2::to_gray("foo"), termcolor2::gray + "foo" + termcolor2::reset
    ));
    expect(
        eq(termcolor2::to_gray(std::string("foo")),
           termcolor2::gray + "foo" + termcolor2::reset)
    );

    expect(eq(
        termcolor2::to_red("foo"), termcolor2::red + "foo" + termcolor2::reset
    ));
    expect(
        eq(termcolor2::to_red(std::string("foo")),
           termcolor2::red + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_green("foo"),
           termcolor2::green + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_green(std::string("foo")),
           termcolor2::green + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_yellow("foo"),
           termcolor2::yellow + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_yellow(std::string("foo")),
           termcolor2::yellow + "foo" + termcolor2::reset)
    );

    expect(eq(
        termcolor2::to_blue("foo"), termcolor2::blue + "foo" + termcolor2::reset
    ));
    expect(
        eq(termcolor2::to_blue(std::string("foo")),
           termcolor2::blue + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_magenta("foo"),
           termcolor2::magenta + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_magenta(std::string("foo")),
           termcolor2::magenta + "foo" + termcolor2::reset)
    );

    expect(eq(
        termcolor2::to_cyan("foo"), termcolor2::cyan + "foo" + termcolor2::reset
    ));
    expect(
        eq(termcolor2::to_cyan(std::string("foo")),
           termcolor2::cyan + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_white("foo"),
           termcolor2::white + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_white(std::string("foo")),
           termcolor2::white + "foo" + termcolor2::reset)
    );
  };

  "test to_color_background_manipulators"_test = [] {
    expect(
        eq(termcolor2::to_on_gray("foo"),
           termcolor2::on_gray + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_on_gray(std::string("foo")),
           termcolor2::on_gray + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_on_red("foo"),
           termcolor2::on_red + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_on_red(std::string("foo")),
           termcolor2::on_red + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_on_green("foo"),
           termcolor2::on_green + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_on_green(std::string("foo")),
           termcolor2::on_green + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_on_yellow("foo"),
           termcolor2::on_yellow + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_on_yellow(std::string("foo")),
           termcolor2::on_yellow + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_on_blue("foo"),
           termcolor2::on_blue + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_on_blue(std::string("foo")),
           termcolor2::on_blue + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_on_magenta("foo"),
           termcolor2::on_magenta + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_on_magenta(std::string("foo")),
           termcolor2::on_magenta + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_on_cyan("foo"),
           termcolor2::on_cyan + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_on_cyan(std::string("foo")),
           termcolor2::on_cyan + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_on_white("foo"),
           termcolor2::on_white + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_on_white(std::string("foo")),
           termcolor2::on_white + "foo" + termcolor2::reset)
    );
  };

  "test to_color_attribute_manipulators"_test = [] {
    expect(eq(
        termcolor2::to_bold("foo"), termcolor2::bold + "foo" + termcolor2::reset
    ));
    expect(
        eq(termcolor2::to_bold(std::string("foo")),
           termcolor2::bold + "foo" + termcolor2::reset)
    );

    expect(eq(
        termcolor2::to_dark("foo"), termcolor2::dark + "foo" + termcolor2::reset
    ));
    expect(
        eq(termcolor2::to_dark(std::string("foo")),
           termcolor2::dark + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_underline("foo"),
           termcolor2::underline + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_underline(std::string("foo")),
           termcolor2::underline + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_blink("foo"),
           termcolor2::blink + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_blink(std::string("foo")),
           termcolor2::blink + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_reverse("foo"),
           termcolor2::reverse + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_reverse(std::string("foo")),
           termcolor2::reverse + "foo" + termcolor2::reset)
    );

    expect(
        eq(termcolor2::to_concealed("foo"),
           termcolor2::concealed + "foo" + termcolor2::reset)
    );
    expect(
        eq(termcolor2::to_concealed(std::string("foo")),
           termcolor2::concealed + "foo" + termcolor2::reset)
    );
  };
}
