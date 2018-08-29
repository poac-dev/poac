#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "../../../include/poac/core/inference.hpp"
#include "../../../include/poac/core/exception.hpp"


BOOST_AUTO_TEST_SUITE(sample)

BOOST_AUTO_TEST_CASE( poac_core_inference_test1 )
{
//    using namespace std;
    using namespace poac::core::infer;
    using namespace poac::core::exception;

    BOOST_CHECK_THROW(apply(std::string("exec"), std::string("nothing"), std::vector<std::string>()), invalid_first_arg);
}

BOOST_AUTO_TEST_SUITE_END()
