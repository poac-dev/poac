#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <poac/core/exception.hpp>


BOOST_AUTO_TEST_CASE( poac_core_exception_test1 )
{
    using namespace poac::core::exception;

    const std::string msg = "hoge";
    try {
        throw invalid_first_arg(msg);
    }
    catch (const invalid_first_arg& e) {
        BOOST_TEST(e.what() == msg);
    }
    catch (...) {
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE( poac_core_exception_test2 )
{
    using namespace poac::core::exception;

    const std::string msg = "hoge";
    try {
        throw invalid_second_arg(msg);
    }
    catch (const invalid_second_arg& e) {
        BOOST_TEST(e.what() == msg);
    }
    catch (...) {
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE( poac_core_exception_test3 )
{
    using namespace poac::core::exception;

    const std::string msg = "hoge";
    try {
        throw error(msg);
    }
    catch (const error& e) {
        BOOST_TEST(e.what() == msg);
    }
    catch (...) {
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE( poac_core_exception_test4 )
{
    using namespace poac::core::exception;

    const std::string msg = "hoge";
    try {
        throw warn(msg);
    }
    catch (const warn& e) {
        BOOST_TEST(e.what() == msg);
    }
    catch (...) {
        BOOST_TEST(false);
    }
}
