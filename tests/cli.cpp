#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <string>
#include <bin/poac/cli.hpp>

BOOST_AUTO_TEST_CASE( poac_core_cli_exec_test )
{
    using bin::poac::exec;
    BOOST_CHECK( exec(std::string("nothing"), std::vector<std::string>()).has_value() );
}
