#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>
#include <map>

#include <poac/io/yaml.hpp>
#include <poac/core/except.hpp>

// std::map<std::string, YAML::Node> get_by_width(const YAML::Node& node, const Args&... args)
BOOST_AUTO_TEST_CASE( poac_io_yaml_get_by_width_test )
{
    using poac::io::yaml::get_by_width;

    {
        YAML::Node node = YAML::Load(
                "hoge: foo\n"
                "fuga: boo");

        const auto res = get_by_width(node, "hoge", "fuga");
        BOOST_TEST(res.at("hoge").as<std::string>() == "foo");
        BOOST_TEST(res.at("fuga").as<std::string>() == "boo");

        BOOST_CHECK_THROW(
                get_by_width(node, "hoge", "nokey"),
                poac::core::except::error
        );
    }
    {
        YAML::Node node = YAML::Load(
                "jbeder/yaml-cpp:\n"
                "  src: github\n"
                "  tag: yaml-cpp-0.6.2\n");

        const auto res = get_by_width(node, "jbeder/yaml-cpp");
        const auto mp = res.as<std::map<std::string, std::string>>();
        BOOST_TEST(mp.at("src") == "github");
        BOOST_TEST(mp.at("tag") == "yaml-cpp-0.6.2");
    }
    {
        YAML::Node node = YAML::Load(
                "include:\n"
                "  - boost_system\n"
                "  - boost_filesystem\n");
        const auto res = get_by_width(node, "include");
        const auto mp = res.as<std::vector<std::string>>();
        BOOST_TEST(mp[0] == "boost_system");
        BOOST_TEST(mp[1] == "boost_filesystem");
    }
}
