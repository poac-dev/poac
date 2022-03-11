#include <boost/ut.hpp>

#include <exception>
#include <string>
#include <utility>

#include <poac/util/pretty.hpp>

int main() {
    using namespace std::literals::string_literals;
    using namespace boost::ut;

    // std::string to_time(const std::string& s)
    "test to_time"_test = [] {
        using poac::util::pretty::to_time;

        expect(eq(to_time("0.5"), "0.50s"s));
        expect(eq(to_time("1.1"), "1s"s));
        expect(eq(to_time("60"), "1m 0s"s));
        expect(eq(to_time("60.1"), "1m 0s"s));
        expect(eq(to_time("60.5"), "1m 0s"s));
        expect(eq(to_time("70"), "1m 10s"s));
        expect(eq(to_time("3600"), "1h 0s"s));
        expect(eq(to_time("3670"), "1h 1m 10s"s));
        expect(eq(to_time("86400"), "1d 0s"s));
        expect(eq(to_time("86470"), "1d 1m 10s"s));
        expect(eq(to_time("90070"), "1d 1h 1m 10s"s));
        expect(eq(to_time("900q70"), "15m 0s"s)); // 900
        expect(throws<std::invalid_argument>([] { to_time("r90"); }));
    };

    // std::pair<float, std::string> to_byte(const float b)
    "test to_byte"_test = [] {
        using poac::util::pretty::to_byte;

        // Using implicit conversion between int and float.
        expect(eq(to_byte(12), "12.00B"s));
        expect(eq(to_byte(1'024), "1.00KB"s));
        expect(eq(to_byte(12'000), "11.72KB"s));
        expect(eq(to_byte(1'000'000), "976.56KB"s));
        expect(eq(to_byte(12'000'000), "11.44MB"s));
        expect(eq(to_byte(1'000'000'000), "953.67MB"s));
        expect(eq(to_byte(12'000'000'000), "11.18GB"s));
        expect(eq(to_byte(1'000'000'000'000), "931.32GB"s));
        expect(eq(to_byte(12'000'000'000'000), "10.91TB"s));
        expect(eq(to_byte(1'000'000'000'000'000), "909.49TB"s));
        expect(eq(to_byte(1'000'000'000'000'000'000), "888.18PB"s));
    };

    // std::string clip_string(const std::string& s, const unsigned long& n)
    "test clip_string"_test = [] {
        using poac::util::pretty::clip_string;

        expect(eq(clip_string("Long sentence", 15), "Long sentence"s));
        expect(eq(clip_string("Long sentence", 10), "Long sente..."s));
    };

    // std::vector<std::string> textwrap(const std::string& text, std::size_t width = 70)
    "test textwrap"_test = [] {
        using poac::util::pretty::textwrap;

        std::string test_case = "This function does not break long words and break on hyphens.";
        expect(eq(
            textwrap(test_case, 15),
            std::vector{
                    "This function"s,
                    "does not break"s,
                    "long words and"s,
                    "break on"s,
                    "hyphens."s
                }
        ));
    };
}
