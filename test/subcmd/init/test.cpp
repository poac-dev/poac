#define BOOST_TEST_MODULE subcmd_init_test
#include <boost/test/included/unit_test.hpp>
namespace bt = boost::unit_test;

// most frequently you implement test cases as a free functions with automatic registration
BOOST_AUTO_TEST_CASE( test1 )
{
    // reports 'error in "test1": test 2 == 1 failed'
    BOOST_TEST( 2 == 1 );
}

//____________________________________________________________________________//

// each test file may contain any number of test cases; each test case has to have unique name
BOOST_AUTO_TEST_CASE( test2 )
{
    int i = 0;

    // reports 'error in "test2": check i == 2 failed [0 != 2]'
    BOOST_TEST( i == 2 );

    BOOST_TEST( i == 0 );
}
