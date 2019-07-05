#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <yaml-cpp/yaml.h>
#include <poac/core/name.hpp>
#include <poac/core/except.hpp>

// std::string basename(const std::string& name)
BOOST_AUTO_TEST_CASE( poac_core_name_basename_test )
{
    using poac::core::name::basename;
    BOOST_CHECK( basename("owner/repo") == "repo" );
    BOOST_CHECK( basename("repo") == "repo" );
}

// std::string slash_to_hyphen(std::string name)
BOOST_AUTO_TEST_CASE( poac_core_name_slash_to_hyphen_test )
{
    using poac::core::name::slash_to_hyphen;
    BOOST_CHECK( slash_to_hyphen("owner/repo") == "owner-repo" );
    BOOST_CHECK( slash_to_hyphen("repo") == "repo" );
}

// std::string hyphen_to_slash(std::string name)
BOOST_AUTO_TEST_CASE( poac_core_name_hyphen_to_slash_test )
{
    using poac::core::name::hyphen_to_slash;
    BOOST_CHECK( hyphen_to_slash("owner-repo") == "owner/repo" );
    BOOST_CHECK( hyphen_to_slash("repo") == "repo" );
}

// std::string to_cache_github(const std::string& name, const std::string& tag)
BOOST_AUTO_TEST_CASE( poac_core_name_to_cache_github_test )
{
    using poac::core::name::to_cache_github;
    BOOST_CHECK( to_cache_github("opencv/opencv", "3.4.2") == "opencv-3.4.2" );
    BOOST_CHECK( to_cache_github("curl/curl", "7_61_0") == "curl-7.61.0" );
    BOOST_CHECK( to_cache_github("matken11235/logger_slack_backend", "v0.1.0") == "logger_slack_backend-0.1.0" );
    BOOST_CHECK( to_cache_github("curl/curl", "curl-7_61_0") == "curl-7_61_0" );
}

// std::string to_cache_poac(const std::string& name, const std::string& ver)
BOOST_AUTO_TEST_CASE( poac_core_name_to_cache_poac_test )
{
    using poac::core::name::to_cache_poac;
    BOOST_CHECK( to_cache_poac("boost/optional", "1.66.0") == "boost-optional-1.66.0" );
}

// std::string cache_to_current(const std::string& name)
BOOST_AUTO_TEST_CASE( poac_core_name_cache_to_current_test )
{
    using poac::core::name::cache_to_current;
    BOOST_CHECK( cache_to_current("opencv-3.4.2") == "opencv" );
    BOOST_CHECK( cache_to_current("opencv") == "opencv" );
}

// std::string to_cache(const std::string& src, const std::string& name, const std::string& version)
BOOST_AUTO_TEST_CASE( poac_core_name_to_cache_test )
{
    using poac::core::name::to_cache;

    BOOST_CHECK( to_cache("poac", "boost/optional", "1.66.0") == "boost-optional-1.66.0" );

    BOOST_CHECK( to_cache("github", "opencv/opencv", "3.4.2") == "opencv-3.4.2" );
    BOOST_CHECK( to_cache("github", "curl/curl", "7_61_0") == "curl-7.61.0" );
    BOOST_CHECK( to_cache("github", "matken11235/logger_slack_backend", "v0.1.0") == "logger_slack_backend-0.1.0" );
    BOOST_CHECK( to_cache("github", "curl/curl", "curl-7_61_0") == "curl-7_61_0" );

    BOOST_CHECK_THROW(
            to_cache("unknown", "", ""),
            poac::core::except::error
    );
}

// std::string to_current(const std::string& src, const std::string& name, const std::string& version)
BOOST_AUTO_TEST_CASE( poac_core_name_to_current_test )
{
    using poac::core::name::to_current;

    BOOST_CHECK( to_current("poac", "owner/repo", "1.0.0") == "owner-repo" );
    BOOST_CHECK( to_current("poac", "repo", "1.0.0") == "repo" );

    BOOST_CHECK( to_current("github", "opencv/opencv", "3.4.2") == "opencv" );
    BOOST_CHECK( to_current("github", "curl/curl", "7_61_0") == "curl" );
    BOOST_CHECK( to_current("github", "matken11235/logger_slack_backend", "v0.1.0") == "logger_slack_backend" );
    BOOST_CHECK( to_current("github", "curl/curl", "curl-7_61_0") == "curl" );

    BOOST_CHECK_THROW(
            to_current("unknown", "", ""),
            poac::core::except::error
    );
}

// std::string get_version(const YAML::Node& node, const std::string& src)
BOOST_AUTO_TEST_CASE( poac_core_name_get_version_test )
{
    using poac::core::name::get_version;

    YAML::Node node1 = YAML::Load("1.0.0");
    BOOST_CHECK( get_version(node1, "github") == "1.0.0" );
    BOOST_CHECK( get_version(node1, "poac") == "1.0.0" );

    YAML::Node node2 = YAML::Load("tag: 1.0.0");
    BOOST_CHECK( get_version(node2, "github") == "1.0.0" );

    YAML::Node node3 = YAML::Load("version: 1.0.0");
    BOOST_CHECK( get_version(node3, "poac") == "1.0.0" );

    BOOST_CHECK_THROW(
            get_version(node1, "unknown"),
            poac::core::except::error
    );
}

// std::pair<std::string, std::string> get_source(const std::string& name)
BOOST_AUTO_TEST_CASE( poac_core_name_get_source_test )
{
    using poac::core::name::get_source;

    const std::pair<std::string, std::string> github_case("github", "opencv/opencv");
    const bool res1 = get_source("github/opencv/opencv") == github_case;
    BOOST_CHECK( res1 );

    const std::pair<std::string, std::string> poac_case("poac", "owner/repo");
    const bool res2 = get_source("owner/repo") == poac_case;
    BOOST_CHECK( res2 );
}

// void validate_package_name(const std::string& s)
BOOST_AUTO_TEST_CASE( poac_core_name_validate_package_name_test )
{
    using poac::core::name::validate_package_name;

    BOOST_CHECK_THROW(
            validate_package_name("/"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("-"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("_"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("1"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("/-"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("-_"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("_2"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("1/"),
            poac::core::except::error
    );

    BOOST_CHECK_THROW(
            validate_package_name("/name"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("-name"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("_name"),
            poac::core::except::error
    );
    BOOST_CHECK_NO_THROW(validate_package_name("1name"));

    BOOST_CHECK_THROW(
            validate_package_name("name/"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("name-"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("name_"),
            poac::core::except::error
    );
    BOOST_CHECK_NO_THROW(validate_package_name("name3"));

    BOOST_CHECK_THROW(
            validate_package_name("na/-me"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("na//me"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("na_-_me"),
            poac::core::except::error
    );
    BOOST_CHECK_NO_THROW(validate_package_name("name-to-name"));
    BOOST_CHECK_NO_THROW(validate_package_name("na23me"));

    BOOST_CHECK_THROW(
            validate_package_name("owner/repo/name"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("owner/repo/name/subname"),
            poac::core::except::error
    );

    BOOST_CHECK_THROW(
            validate_package_name("na$me"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("nam()e"),
            poac::core::except::error
    );
    BOOST_CHECK_THROW(
            validate_package_name("namße"),
            poac::core::except::error
    );

    BOOST_CHECK_NO_THROW( validate_package_name("poacpm/poac") );
}
