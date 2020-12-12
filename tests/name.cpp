#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/core/name.hpp>

// std::string to_cache(std::string name, const std::string& version)
BOOST_AUTO_TEST_CASE( poac_core_name_to_cache_test )
{
    using poac::core::name::to_cache;

    BOOST_CHECK( to_cache("boost/optional", "1.66.0") == "boost-optional-1.66.0" );
    BOOST_CHECK( to_cache("opencv/opencv", "3.4.2") == "opencv-opencv-3.4.2" );
    BOOST_CHECK( to_cache("ken-matsui/logger_slack_backend", "0.1.0") == "ken-matsui-logger_slack_backend-0.1.0" );
    BOOST_CHECK( to_cache("curl/curl", "curl-7_61_0") == "curl-curl-curl-7_61_0" );
}

// std::string to_current(std::string name)
BOOST_AUTO_TEST_CASE( poac_core_name_to_current_test )
{
    using poac::core::name::to_current;

    BOOST_CHECK( to_current("owner/repo") == "owner-repo" );
    BOOST_CHECK( to_current("opencv/opencv") == "opencv-opencv" );
    BOOST_CHECK( to_current("ken-matsui/logger_slack_backend") == "ken-matsui-logger_slack_backend" );
    BOOST_CHECK( to_current("curl/curl") == "curl-curl" );
}

// std::optional<core::except::Error> validate_package_name(std::string_view s)
BOOST_AUTO_TEST_CASE( poac_core_name_validate_package_name_test )
{
    using poac::core::name::validate_package_name;

    BOOST_CHECK( validate_package_name("na$me").is_err() );
    BOOST_CHECK( validate_package_name("nam()e").is_err() );
    BOOST_CHECK( validate_package_name("namße").is_err() );

    BOOST_CHECK( !validate_package_name("poacpm/poac-api").is_err() );
    BOOST_CHECK( !validate_package_name("poacpm/poac_api").is_err() );
    BOOST_CHECK( !validate_package_name("poacpm/poac").is_err() );
}
