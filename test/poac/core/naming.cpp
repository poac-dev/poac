#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <yaml-cpp/yaml.h>
#include <poac/core/naming.hpp>
#include <poac/core/except.hpp>


// std::string basename(const std::string& name)
BOOST_AUTO_TEST_CASE( poac_core_naming_basename_test )
{
    using namespace poac::core;
    BOOST_TEST( naming::basename("user/repo") == "repo" );
    BOOST_TEST( naming::basename("repo") == "repo" );
}

// std::string slash_to_hyphen(std::string name)
BOOST_AUTO_TEST_CASE( poac_core_naming_slash_to_hyphen_test )
{
    using namespace poac::core;
    BOOST_TEST( naming::slash_to_hyphen("user/repo") == "user-repo" );
    BOOST_TEST( naming::slash_to_hyphen("repo") == "repo" );
}

// std::string hyphen_to_slash(std::string name)
BOOST_AUTO_TEST_CASE( poac_core_naming_hyphen_to_slash_test )
{
    using namespace poac::core;
    BOOST_TEST( naming::hyphen_to_slash("user-repo") == "user/repo" );
    BOOST_TEST( naming::hyphen_to_slash("repo") == "repo" );
}

// std::string to_cache_github(const std::string& name, const std::string& tag)
BOOST_AUTO_TEST_CASE( poac_core_naming_to_cache_github_test )
{
    using namespace poac::core;
    BOOST_TEST( naming::to_cache_github("opencv/opencv", "3.4.2") == "opencv-3.4.2" );
    BOOST_TEST( naming::to_cache_github("curl/curl", "7_61_0") == "curl-7.61.0" );
    BOOST_TEST( naming::to_cache_github("matken11235/logger_slack_backend", "v0.1.0") == "logger_slack_backend-0.1.0" );
    BOOST_TEST( naming::to_cache_github("curl/curl", "curl-7_61_0") == "curl-7_61_0" );
}

// std::string to_cache_poac(const std::string& name, const std::string& ver)
BOOST_AUTO_TEST_CASE( poac_core_naming_to_cache_poac_test )
{
    using namespace poac::core;
    BOOST_TEST( naming::to_cache_poac("boost/optional", "1.66.0") == "boost-optional-1.66.0" );
}

// std::string cache_to_current(const std::string& name)
BOOST_AUTO_TEST_CASE( poac_core_naming_cache_to_current_test )
{
    using namespace poac::core;
    BOOST_TEST( naming::cache_to_current("opencv-3.4.2") == "opencv" );
    BOOST_TEST( naming::cache_to_current("opencv") == "opencv" );
}

// std::string to_cache(const std::string& src, const std::string& name, const std::string& version)
BOOST_AUTO_TEST_CASE( poac_core_naming_to_cache_test )
{
    using namespace poac::core;
    BOOST_TEST( naming::to_cache("poac", "boost/optional", "1.66.0") == "boost-optional-1.66.0" );

    BOOST_TEST( naming::to_cache("github", "opencv/opencv", "3.4.2") == "opencv-3.4.2" );
    BOOST_TEST( naming::to_cache("github", "curl/curl", "7_61_0") == "curl-7.61.0" );
    BOOST_TEST( naming::to_cache("github", "matken11235/logger_slack_backend", "v0.1.0") == "logger_slack_backend-0.1.0" );
    BOOST_TEST( naming::to_cache("github", "curl/curl", "curl-7_61_0") == "curl-7_61_0" );

    BOOST_CHECK_THROW(
            naming::to_cache("unknown", "", ""),
            except::error
    );
}
