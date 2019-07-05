#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string_view>
#include <poac/core/except.hpp>

BOOST_AUTO_TEST_CASE( poac_core_except_error_test )
{
    using poac::core::except::error;

    const std::string msg = "hoge";
    try {
        throw error(msg);
    }
    catch (const error& e) {
        BOOST_CHECK(e.what() == msg);
    }

    try {
        std::string_view msg1 = "ge,h";
        const std::string msg2 = "oge: ";
        const unsigned int msg3 = 10;
        throw error("ho", msg1, msg2, msg3);
    }
    catch (const error& e) {
        BOOST_CHECK(e.what() == "hoge,hoge: 10");
    }
}
