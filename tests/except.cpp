#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <string_view>
#include <poac/core/except.hpp>

BOOST_AUTO_TEST_CASE( poac_core_except_error_test )
{
    using poac::core::except::error;

    const std::string msg = "hoge";
    try {
        throw error(msg);
    } catch (const error& e) {
        BOOST_CHECK(e.what() == msg);
    }
}
