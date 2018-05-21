#define BOOST_TEST_MODULE subcmd_init_test
#include <boost/test/included/unit_test.hpp>
namespace bt = boost::unit_test;

// each test file may contain any number of test cases; each test case has to have unique name
BOOST_AUTO_TEST_CASE( test1 )
{
    int i = 0;
    BOOST_TEST( i == 0 );
}
