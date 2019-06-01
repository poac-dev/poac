#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

#include <poac/util/misc.hpp>


// std::vector<std::string> split(const std::string& raw, const std::string& delim)
BOOST_AUTO_TEST_CASE( poac_util_misc_split_test )
{
    using poac::util::misc::split;

    const std::string test_case = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod ...";

    std::vector<std::string> comp{ "Lorem ipsum dolor sit amet", " consectetur adipiscing elit", " sed do eiusmod ..." };
    BOOST_TEST( split(test_case, ",") == comp );

    comp = { "Lorem ", "psum dolor s", "t amet, consectetur ad", "p", "sc", "ng el", "t, sed do e", "usmod ..." };
    BOOST_TEST( split(test_case, "i") == comp );
}
