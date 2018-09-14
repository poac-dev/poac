#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include <poac/core/inference.hpp>
#include <poac/core/exception.hpp>


// void check_arguments(const std::vector<std::string>& argv)
BOOST_AUTO_TEST_CASE( poac_subcmd_login_test1 )
{
    using namespace std;
    using namespace poac;
    using namespace poac::core::exception;

    subcmd::login test{};

    BOOST_CHECK_THROW(
            test(vector<string>{}),
            invalid_second_arg
    );
    BOOST_CHECK_THROW(
            test(vector<string>{"arg1", "arg2"}),
            invalid_second_arg
    );
    // std::regex_match == false
//    BOOST_CHECK_THROW(
//            test(vector<string>{"arg1"}),
//            invalid_second_arg
//    );
    // 089ee282-2eb2-4618-98ae-52543f3803e0
}
