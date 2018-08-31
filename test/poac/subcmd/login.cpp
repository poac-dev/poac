#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include <poac/core/inference.hpp>
#include <poac/core/exception.hpp>


BOOST_AUTO_TEST_CASE( poac_subcmd_login_test1 )
{
    using namespace std;
    using namespace poac;
    using namespace poac::core::exception;

    subcmd::login test;

    BOOST_CHECK_THROW(test(vector<string>()), invalid_second_arg);
}
