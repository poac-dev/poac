#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "../../src/include/poac/core/inference.hpp"
#include "../../src/include/poac/core/except.hpp"


BOOST_AUTO_TEST_CASE( test1 )
{
    using namespace std;
    using namespace poac::core::inference;
    using namespace poac::core;

    BOOST_CHECK_THROW(apply("exec", "nothing", vector<string>()), invalid_first_argument);
}
