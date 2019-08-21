#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <string>
#include <poac/core/cli.hpp>

BOOST_AUTO_TEST_CASE( poac_core_cli_execute_test )
{
    using poac::core::cli::exec;
    BOOST_CHECK( exec(std::string("nothing"), std::vector<std::string>()).has_value() );
}
