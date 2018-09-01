#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <yaml-cpp/yaml.h>

#include <poac/util/package.hpp>


// std::string basename(const std::string& name)
BOOST_AUTO_TEST_CASE( poac_util_package_test1 )
{
    using namespace poac::util;
    BOOST_TEST( package::basename("username/repository") == "repository" );
    BOOST_TEST( package::basename("repository") == "repository" );
}
// std::string github_conv_pkgname(const std::string& name, const std::string& tag)
BOOST_AUTO_TEST_CASE( poac_util_package_test2 )
{
    using namespace poac::util;
    BOOST_TEST( package::github_conv_pkgname("opencv/opencv", "3.4.2") == "opencv-3.4.2" );
    BOOST_TEST( package::github_conv_pkgname("curl/curl", "7_61_0") == "curl-7.61.0" );
    BOOST_TEST( package::github_conv_pkgname("matken11235/logger_slack_backend", "v0.1.0") == "logger_slack_backend-0.1.0" );
    BOOST_TEST( package::github_conv_pkgname("curl/curl", "curl-7_61_0") == "curl-7_61_0" );
    // TODO: not yet 5
    //BOOST_TEST( package::github_conv_pkgname("boostorg/optional", "boost-1.67.0") == "boost-optional-1.67.0" );
}
// std::string cache_to_current(const std::string& pkgname)
BOOST_AUTO_TEST_CASE( poac_util_package_test3 )
{
    using namespace poac::util;
    BOOST_TEST( package::cache_to_current("opencv-3.4.2") == "opencv" );
    BOOST_TEST( package::cache_to_current("opencv") == "opencv" );
}
// std::string poac_conv_pkgname(const std::string& name, const std::string& tag)
BOOST_AUTO_TEST_CASE( poac_util_package_test4 )
{
    using namespace poac::util;
    BOOST_TEST( package::poac_conv_pkgname("opencv", "3.4.2") == "opencv-3.4.2" );
}
// std::string get_source(const YAML::Node& node)
BOOST_AUTO_TEST_CASE( poac_util_package_test5 )
{
    using namespace poac::util;

    const YAML::Node node = YAML::Load("src: github");
    BOOST_TEST( package::get_source(node) == "github" );

    const YAML::Node node2 = YAML::Load("");
    BOOST_TEST( package::get_source(node2) == "poac" );
}
