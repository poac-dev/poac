#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <poac/core/name.hpp>

// std::string to_cache(std::string name, const std::string& version)
BOOST_AUTO_TEST_CASE( poac_core_name_to_cache_test )
{
    using poac::core::name::to_cache;

    BOOST_CHECK( to_cache("boost/optional", "1.66.0") == "boost-optional-1.66.0" );
    BOOST_CHECK( to_cache("opencv/opencv", "3.4.2") == "opencv-opencv-3.4.2" );
    BOOST_CHECK( to_cache("matken11235/logger_slack_backend", "0.1.0") == "matken11235-logger_slack_backend-0.1.0" );
    BOOST_CHECK( to_cache("curl/curl", "curl-7_61_0") == "curl-curl-curl-7_61_0" );
}

// std::string to_current(std::string name)
BOOST_AUTO_TEST_CASE( poac_core_name_to_current_test )
{
    using poac::core::name::to_current;

    BOOST_CHECK( to_current("owner/repo") == "owner-repo" );
    BOOST_CHECK( to_current("opencv/opencv") == "opencv-opencv" );
    BOOST_CHECK( to_current("matken11235/logger_slack_backend") == "matken11235-logger_slack_backend" );
    BOOST_CHECK( to_current("curl/curl") == "curl-curl" );
}

// std::optional<core::except::Error> validate_package_name(std::string_view s)
BOOST_AUTO_TEST_CASE( poac_core_name_validate_package_name_test )
{
    using poac::core::name::validate_package_name;

    BOOST_CHECK( validate_package_name("na$me").has_value() );
    BOOST_CHECK( validate_package_name("nam()e").has_value() );
    BOOST_CHECK( validate_package_name("namße").has_value() );

    BOOST_CHECK( !validate_package_name("poacpm/poac-api").has_value() );
    BOOST_CHECK( !validate_package_name("poacpm/poac_api").has_value() );
    BOOST_CHECK( !validate_package_name("poacpm/poac").has_value() );
}
