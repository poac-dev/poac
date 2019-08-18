#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include "macros.hpp"

BOOST_AUTO_TEST_CASE( support_test_boost_check_throw_msg_test )
{
    BOOST_CHECK_THROW_MSG( throw std::runtime_error("foo"), std::runtime_error, "foo" );
    BOOST_CHECK_THROW_MSG( throw std::range_error("bar"), std::range_error, "bar" );
}
