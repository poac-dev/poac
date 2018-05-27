#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include "../../include/poac/inference.hpp"


BOOST_AUTO_TEST_CASE( test1 )
{
    BOOST_CHECK_THROW(poac::inference::apply("exec", "nothing"), std::invalid_argument);
}
