#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "test_ofstream.hpp"
#include <poac/io/filesystem.hpp>

BOOST_AUTO_TEST_CASE( support_test_ofstream_test )
{
    {
        support::test_ofstream ofs("fuga.txt");
        BOOST_CHECK( poac::io::filesystem::exists("fuga.txt") );
    }
    BOOST_CHECK( !poac::io::filesystem::exists("fuga.txt") );
}
