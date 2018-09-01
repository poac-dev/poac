#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

#include <poac/core/exception.hpp>
#include <poac/subcmd/init.hpp>


// void check_arguments(const std::vector<std::string>& argv)
BOOST_AUTO_TEST_CASE( poac_subcmd_init_test1 )
{
    using namespace std;
    using namespace poac;
    using namespace poac::core::exception;

    BOOST_CHECK_THROW(
            subcmd::init{}(
                    vector<string>{"dummy"}
            ),
            invalid_second_arg
    );
}
// void conv_prohibit_char(std::string& s)
BOOST_AUTO_TEST_CASE( poac_subcmd_init_test2 )
{
    using namespace std;
    using namespace poac;
    using namespace poac::core::exception;

    subcmd::init test{};

    string s("Hoge-Foo");
    test.conv_prohibit_char(s);

    BOOST_TEST(s == "hoge_foo");
}
