#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include <poac/core/cli.hpp>
#include <poac/core/except.hpp>

BOOST_AUTO_TEST_CASE( poac_core_cli_execute_test )
{
    using poac::core::cli::exec;
    BOOST_CHECK( static_cast<bool>(exec(std::string("nothing"), std::vector<std::string>())) );
}
