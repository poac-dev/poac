#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string_view>
#include <poac/core/except.hpp>

BOOST_AUTO_TEST_CASE( poac_core_except_test1 )
{
    using poac::core::except::invalid_first_arg;

    const std::string msg = "hoge";
    try {
        throw invalid_first_arg(msg);
    }
    catch (const invalid_first_arg& e) {
        BOOST_TEST(e.what() == msg);
    }
}

BOOST_AUTO_TEST_CASE( poac_core_except_test2 )
{
    using poac::core::except::invalid_second_arg;

    const std::string msg = "hoge";
    try {
        throw invalid_second_arg(msg);
    }
    catch (const invalid_second_arg& e) {
        BOOST_TEST(e.what() == msg);
    }
}

BOOST_AUTO_TEST_CASE( poac_core_except_test3 )
{
    using poac::core::except::error;

    const std::string msg = "hoge";
    try {
        throw error(msg);
    }
    catch (const error& e) {
        BOOST_TEST(e.what() == msg);
    }
}

BOOST_AUTO_TEST_CASE( poac_core_except_test4 )
{
    using poac::core::except::error;

    try {
        std::string_view msg1 = "ge,h";
        const std::string msg2 = "oge: ";
        const unsigned int msg3 = 10;
        throw error("ho", msg1, msg2, msg3);
    }
    catch (const error& e) {
        BOOST_TEST(e.what() == "hoge,hoge: 10");
    }
}
