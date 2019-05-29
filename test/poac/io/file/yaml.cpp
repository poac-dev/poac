#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>
#include <map>

#include <poac/io/file/yaml.hpp>


BOOST_AUTO_TEST_CASE( poac_io_file_yaml_gets )
{
    using namespace std;
    using namespace poac::io::file;
    YAML::Node node = YAML::Load("hoge: foo\n"
                                 "fuga: boo");
    if (const auto res = yaml::get_by_width(node, "hoge", "fuga"))
    {
        BOOST_TEST( res->at("hoge").as<string>() == "foo" );
        BOOST_TEST( res->at("fuga").as<string>() == "boo" );
    }
    else
    {
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE( poac_io_file_yaml_gets2 )
{
    using namespace poac::io::file;
    YAML::Node node = YAML::Load("hoge: foo\n"
                                 "fuga: boo");
    if (const auto res = yaml::get_by_width(node, "hoge", "nokey"))
    {
        BOOST_TEST(false);
    }
    else
    {
        BOOST_TEST(true);
    }
}
// Read std::map
BOOST_AUTO_TEST_CASE( poac_io_file_yaml_gets3 )
{
    using namespace std;
    using namespace poac::io::file;
    YAML::Node node = YAML::Load("jbeder/yaml-cpp:\n"
                                 "  src: github\n"
                                 "  tag: yaml-cpp-0.6.2\n");
    if (const auto res = yaml::get_by_width(node, "jbeder/yaml-cpp"))
    {
        const auto mp = res->at("jbeder/yaml-cpp").as<map<string, string>>();
        BOOST_TEST( mp.at("src") == "github" );
        BOOST_TEST( mp.at("tag") == "yaml-cpp-0.6.2" );
    }
    else
    {
        BOOST_TEST(false);
    }
}
// Read std::vector
BOOST_AUTO_TEST_CASE( poac_io_file_yaml_gets4 )
{
    using namespace std;
    using namespace poac::io::file;
    YAML::Node node = YAML::Load("include:\n"
                                 "  - boost_system\n"
                                 "  - boost_filesystem\n");
    if (const auto res = yaml::get_by_width(node, "include"))
    {
        const auto mp = res->at("include").as<vector<string>>();
        BOOST_TEST( mp[0] == "boost_system" );
        BOOST_TEST( mp[1] == "boost_filesystem" );
    }
    else
    {
        BOOST_TEST(false);
    }
}
