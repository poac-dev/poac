#include <boost/ut.hpp>

// std
#include <string>
#include <vector>

// internal
#include <poac/util/misc.hpp>

int main() {
    using namespace std::literals::string_literals;
    using namespace boost::ut;

    // std::vector<std::string> split(const std::string& raw, const std::string& delim)
    "test split"_test = [] {
        using poac::util::misc::split;

        const std::string test_case = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod ...";

        std::vector<std::string> excepted{ "Lorem ipsum dolor sit amet", " consectetur adipiscing elit", " sed do eiusmod ..." };
        expect(eq(split(test_case, ","), excepted));

        excepted = { "Lorem ", "psum dolor s", "t amet, consectetur ad", "p", "sc", "ng el", "t, sed do e", "usmod ..." };
        expect(eq(split(test_case, "i"), excepted));
    };
}
