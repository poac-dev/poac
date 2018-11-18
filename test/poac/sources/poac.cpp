#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

#include <poac/core/exception.hpp>
#include <poac/sources/poac.hpp>


// void check_arguments(const std::vector<std::string>& argv)
BOOST_AUTO_TEST_CASE( poac_sources_poac_test1 )
{
    using namespace std;
    using namespace poac;
    using namespace poac::core::exception;

    if (const auto ver = sources::poac::decide_version("boost/config", ">=1.66.0")) {
        std::cout << "Decide version: " << *ver << std::endl;
    }
//    BOOST_CHECK_THROW(
//            subcmd::init{}(
//                    vector<string>{"dummy"}
//            ),
//            invalid_second_arg
//    );
}
