#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>
#include "test_ofstream.hpp"
#include <filesystem>

BOOST_AUTO_TEST_CASE( support_test_ofstream_test )
{
    {
        support::test_ofstream ofs("fuga.txt");
        BOOST_CHECK( std::filesystem::exists("fuga.txt") );
    }
    BOOST_CHECK( !std::filesystem::exists("fuga.txt") );
}
