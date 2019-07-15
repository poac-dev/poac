#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <poac/io/yaml.hpp>
#include <poac/core/except.hpp>

// std::optional<T> get(const YAML::Node& node)
// std::optional<T> get(const YAML::Node& node, Args&&... args)
// bool get(const YAML::Node& node, Args&&... args)
BOOST_AUTO_TEST_CASE( poac_io_yaml_get_test )
{
    using poac::io::yaml::get;

    // std::optional<T> get(const YAML::Node& node)
    {
        YAML::Node node = YAML::Load("bar");
        BOOST_CHECK( get<std::string>(node).value() == "bar" );
    }
    // std::optional<T> get(const YAML::Node& node, Args&&... args)
    {
        YAML::Node node = YAML::Load(
                "hoge:\n"
                "  fuga:\n"
                "    foo: bar");

        BOOST_CHECK( get<std::string>(node, "hoge", "fuga", "foo").value() == "bar" );
        BOOST_CHECK_NO_THROW( get<std::string>(node, "hoge", "fuga", "unknown") );
        BOOST_CHECK( !get<std::string>(node, "hoge", "unknown", "fuga").has_value() );
    }
    // bool get(const YAML::Node& node, Args&&... args)
    {
        YAML::Node node = YAML::Load(
                "hoge:\n"
                "  fuga:\n"
                "    foo: true");

        BOOST_CHECK( get(node, "hoge", "fuga", "foo") );
        BOOST_CHECK_NO_THROW( get(node, "hoge", "fuga", "unknown") );
        BOOST_CHECK( !get(node, "hoge", "unknown", "fuga") );
    }
    {
        YAML::Node node = YAML::Load(
                "hoge:\n"
                "  fuga:\n"
                "    foo: false");

        BOOST_CHECK( !get(node, "hoge", "fuga", "foo") );
        BOOST_CHECK_NO_THROW( get(node, "hoge", "fuga", "unknown") );
        BOOST_CHECK( !get(node, "hoge", "unknown", "fuga") );
    }
}

// bool contains(const YAML::Node& node, Args&&... args)
BOOST_AUTO_TEST_CASE( poac_io_yaml_contains_test )
{
    using poac::io::yaml::contains;

    YAML::Node node = YAML::Load(
            "hoge: foo\n"
            "fuga: boo");

    BOOST_CHECK( contains(node, "hoge") );
    BOOST_CHECK( contains(node, "hoge", "fuga") );
    BOOST_CHECK( !contains(node, "unknown") );
    BOOST_CHECK( !contains(node, "hoge", "unknown") );
}

// YAML::Node load_config()
// auto load_config(const Args&... args)
//BOOST_AUTO_TEST_CASE( poac_io_yaml_load_config_test )
//{
//    namespace fs = boost::filesystem;
//    using poac::io::yaml::detail::load_config;
//
//    BOOST_CHECK_THROW(
//            load_config(),
//            poac::core::except::error
//    );
//
//    const fs::path config_path = fs::current_path() / "poac.yml";
//    {
//        std::ofstream ofs(config_path.string());
//        ofs << "hoge: foo\n"
//               "fuga: boo";
//    }
//
//    BOOST_CHECK_NO_THROW( load_config() );
//
//    const auto res = load_config("hoge", "fuga");
//    BOOST_CHECK( res.at("hoge").as<std::string>() == "foo" );
//    BOOST_CHECK( res.at("fuga").as<std::string>() == "boo" );
//
//    BOOST_CHECK_THROW(
//            load_config("hoge", "nokey"),
//            poac::core::except::error
//    );
//
//    fs::remove(config_path);
//}

// std::optional<Config::Build::System> to_build_system(const std::optional<std::string>& str)
BOOST_AUTO_TEST_CASE( poac_io_yaml_detail_to_build_system_test )
{
    using poac::io::yaml::detail::to_build_system;
    using poac::io::yaml::Config;

    BOOST_CHECK( to_build_system(std::nullopt) == Config::Build::System::Poac );
    BOOST_CHECK( to_build_system("poac") == Config::Build::System::Poac );
    BOOST_CHECK( to_build_system("cmake") == Config::Build::System::CMake );
    BOOST_CHECK( !to_build_system("unknown").has_value() );
}

// std::optional<Config::Test::Framework> to_test_framework(const std::optional<std::string>& str)
BOOST_AUTO_TEST_CASE( poac_io_yaml_detail_to_test_framework_test )
{
    using poac::io::yaml::detail::to_test_framework;
    using poac::io::yaml::Config;

    BOOST_CHECK( !to_test_framework(std::nullopt).has_value() );
    BOOST_CHECK( to_test_framework("boost") == Config::Test::Framework::Boost );
    BOOST_CHECK( to_test_framework("google") == Config::Test::Framework::Google );
    BOOST_CHECK( !to_test_framework("unknown").has_value() );
}

// std::optional<YAML::Node> load_yaml(const boost::filesystem::path& base)
BOOST_AUTO_TEST_CASE( poac_io_yaml_detail_load_yaml_test )
{
    namespace fs = boost::filesystem;
    using poac::io::yaml::detail::load_yaml;

    const fs::path config_path = fs::current_path() / "poac.yml";

    BOOST_CHECK( !load_yaml(config_path.string()).has_value() );

    std::ofstream(config_path.string());
    BOOST_CHECK( load_yaml(config_path.string()).has_value() );
    fs::remove(config_path);
}

// std::optional<std::string> validate_config(const boost::filesystem::path& base = boost::filesystem::current_path(ec))
BOOST_AUTO_TEST_CASE( poac_io_yaml_detail_validate_config_test )
{
    namespace fs = boost::filesystem;
    using poac::io::yaml::detail::validate_config;

    BOOST_CHECK( !validate_config().has_value() );

    const fs::path config_path = fs::current_path() / "poac.yml";
    std::ofstream(config_path.string());

    BOOST_CHECK( validate_config().has_value() );

    fs::remove(config_path);
}

// std::optional<YAML::Node> load_config(const boost::filesystem::path& base)
BOOST_AUTO_TEST_CASE( poac_io_yaml_detail_load_config_test )
{
    namespace fs = boost::filesystem;
    using poac::io::yaml::detail::load_config;

    BOOST_CHECK( !load_config(fs::current_path()).has_value() );

    const fs::path config_path = fs::current_path() / "poac.yml";
    std::ofstream(config_path.string());

    BOOST_CHECK( load_config(fs::current_path()).has_value() );

    fs::remove(config_path);
}

// std::string load_timestamp()
BOOST_AUTO_TEST_CASE( poac_io_yaml_load_timestamp_test )
{
    namespace fs = boost::filesystem;
    using poac::io::yaml::load_timestamp;

    BOOST_CHECK_THROW(
            load_timestamp(),
            poac::core::except::error
    );

    const fs::path config_path = fs::current_path() / "poac.yml";
    std::ofstream(config_path.string());

    BOOST_CHECK_NO_THROW( load_timestamp() );

    fs::remove(config_path);
}
