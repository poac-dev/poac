#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include <poac/core/infer.hpp>
#include <poac/core/except.hpp>

BOOST_AUTO_TEST_CASE( poac_core_infer_execute_test )
{
    using poac::core::infer::execute;

    BOOST_CHECK_THROW(
            execute(
                    std::string("nothing"),
                    std::vector<std::string>()
            ),
            poac::core::except::invalid_first_arg
    );
}
