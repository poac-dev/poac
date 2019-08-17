#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <stdexcept>
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

// find_force(const toml::basic_value<C, M, V>& v, const toml::key& key)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_find_force_test )
{
    using toml::toml_literals::operator""_toml;
    using poac::io::config::detail::find_force;

    BOOST_CHECK( find_force<std::string>("name = \"poac\""_toml, "name") == "poac" );
    BOOST_CHECK_THROW( find_force<int>("name = \"poac\""_toml, "name"), toml::type_error );
    BOOST_CHECK_THROW( find_force<std::string>("name = \"poac\""_toml, "unknown"), std::out_of_range );
}

// find_enum(const toml::basic_value<C, M, V>& v, const toml::key& key, std::vector<T>&& pv)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_find_enum_test )
{
    using toml::toml_literals::operator""_toml;
    using poac::io::config::detail::find_enum;

    BOOST_CHECK( find_enum<std::string>("name = \"poac\""_toml, "name", {"poac"}) == "poac" );
    BOOST_CHECK_THROW( find_enum<std::string>("name = \"poac\""_toml, "name", {"foo"}), toml::type_error );
    BOOST_CHECK_THROW( find_enum<int>("name = \"poac\""_toml, "name", {2}), toml::type_error );
    BOOST_CHECK_THROW( find_enum<std::string>("name = \"poac\""_toml, "unknown", {"poac"}), std::out_of_range );
}
