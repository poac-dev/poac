#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string>

#include <poac/io/config.hpp>
#include <poac/core/except.hpp>

// [[noreturn]] inline void rethrow_bad_cast(const std::string& what)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_rethrow_bad_cast_test )
{
    using poac::io::config::detail::rethrow_bad_cast;
    try {
        rethrow_bad_cast("[error] toml::value bad_cast to string\n"
                         " --> poac.toml\n"
                         " 2 | name = []\n"
                         "   |        ~~ the actual type is array");
    } catch (const toml::type_error& e) {
        BOOST_CHECK( std::string(e.what()) ==
            "[error] value type should be string\n"
            " --> poac.toml\n"
            " 2 | name = []\n"
            "   |        ~~ the actual type is array" );
    }
}
