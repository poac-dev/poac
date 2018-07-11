#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "../../../src/include/poac/subcmd/login.hpp"
#include "../../src/include/poac/core/except.hpp"


BOOST_AUTO_TEST_CASE( subcmd_login_test1 )
{
    using namespace std;
    using namespace poac::subcmd;
    using namespace poac::core;

    login test;
    BOOST_CHECK_THROW(test(vector<string>({""})), invalid_second_argument);
}
