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
        BOOST_CHECK( !static_cast<bool>(get<std::string>(node, "hoge", "unknown", "fuga")) );
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

// T get_with_throw(const YAML::Node& node)
// T get_with_throw(const YAML::Node& node, const std::string& arg)
BOOST_AUTO_TEST_CASE( poac_io_yaml_get_with_throw_test )
{
    using poac::io::yaml::get_with_throw;

    // T get_with_throw(const YAML::Node& node)
    {
        YAML::Node node = YAML::Load("bar");

        BOOST_CHECK( get_with_throw<std::string>(node) == "bar" );
        BOOST_CHECK_THROW(
                get_with_throw<int>(node),
                poac::core::except::error
        );
    }
    // T get_with_throw(const YAML::Node& node, const std::string& arg)
    {
        YAML::Node node = YAML::Load("hoge: bar");

        BOOST_CHECK( get_with_throw<std::string>(node, "hoge") == "bar" );
        BOOST_CHECK_THROW(
                get_with_throw<std::string>(node, "unknown"),
                poac::core::except::error
        );
        BOOST_CHECK_THROW(
                get_with_throw<int>(node),
                poac::core::except::error
        );
    }
}

// std::map<std::string, YAML::Node> get_by_width(const YAML::Node& node, const Args&... args)
// YAML::Node get_by_width(const YAML::Node& node, const Arg& args)
BOOST_AUTO_TEST_CASE( poac_io_yaml_get_by_width_test )
{
    using poac::io::yaml::get_by_width;

    {
        YAML::Node node = YAML::Load(
                "hoge: foo\n"
                "fuga: boo");

        const auto res = get_by_width(node, "hoge", "fuga");
        BOOST_CHECK( res.at("hoge").as<std::string>() == "foo" );
        BOOST_CHECK( res.at("fuga").as<std::string>() == "boo" );

        BOOST_CHECK_THROW(
                get_by_width(node, "hoge", "nokey"),
                poac::core::except::error
        );
    }
    {
        YAML::Node node = YAML::Load(
                "matken11235/semver:\n"
                "  src: github\n"
                "  tag: 1.2.0\n");

        const auto res = get_by_width(node, "matken11235/semver");
        const auto mp = res.as<std::map<std::string, std::string>>();
        BOOST_CHECK( mp.at("src") == "github" );
        BOOST_CHECK( mp.at("tag") == "1.2.0" );

        BOOST_CHECK_THROW(
                get_by_width(node, "unknown"),
                poac::core::except::error
        );
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

// std::optional<std::map<std::string, YAML::Node>> get_by_width_opt(const YAML::Node& node, const Args&... args)
BOOST_AUTO_TEST_CASE( poac_io_yaml_get_by_width_opt_test )
{
    using poac::io::yaml::get_by_width_opt;

    YAML::Node node = YAML::Load(
            "hoge: foo\n"
            "fuga: boo");

    const auto res = get_by_width_opt(node, "hoge", "fuga");
    BOOST_CHECK( res->at("hoge").as<std::string>() == "foo" );
    BOOST_CHECK( res->at("fuga").as<std::string>() == "boo" );

    BOOST_CHECK_NO_THROW( get_by_width_opt(node, "hoge", "nokey") );
    BOOST_CHECK( !static_cast<bool>(get_by_width_opt(node, "hoge", "nokey")) );
}

// std::optional<std::string> exists_config(const boost::filesystem::path& base)
// std::optional<std::string> exists_config()
BOOST_AUTO_TEST_CASE( poac_io_yaml_exists_config_test )
{
    namespace fs = boost::filesystem;
    using poac::io::yaml::exists_config;

    BOOST_CHECK( !static_cast<bool>(exists_config(fs::current_path())) );
    BOOST_CHECK( !static_cast<bool>(exists_config()) );

    const fs::path config_path = fs::current_path() / "poac.yml";
    std::ofstream(config_path.string());

    BOOST_CHECK( static_cast<bool>(exists_config(fs::current_path())) );
    BOOST_CHECK( static_cast<bool>(exists_config()) );
    BOOST_CHECK( exists_config(fs::current_path()).value() == config_path );
    BOOST_CHECK( exists_config().value() == config_path );

    fs::remove(config_path);
}

// YAML::Node load_config()
// auto load_config(const Args&... args)
BOOST_AUTO_TEST_CASE( poac_io_yaml_load_config_test )
{
    namespace fs = boost::filesystem;
    using poac::io::yaml::load_config;

    BOOST_CHECK_THROW(
            load_config(),
            poac::core::except::error
    );

    const fs::path config_path = fs::current_path() / "poac.yml";
    {
        std::ofstream ofs(config_path.string());
        ofs << "hoge: foo\n"
               "fuga: boo";
    }

    BOOST_CHECK_NO_THROW( load_config() );

    const auto res = load_config("hoge", "fuga");
    BOOST_CHECK( res.at("hoge").as<std::string>() == "foo" );
    BOOST_CHECK( res.at("fuga").as<std::string>() == "boo" );

    BOOST_CHECK_THROW(
            load_config("hoge", "nokey"),
            poac::core::except::error
    );

    fs::remove(config_path);
}

// std::optional<YAML::Node> load_config_by_dir(const boost::filesystem::path& base)
BOOST_AUTO_TEST_CASE( poac_io_yaml_load_config_by_dir_test )
{
    namespace fs = boost::filesystem;
    using poac::io::yaml::load_config_by_dir;

    BOOST_CHECK( !static_cast<bool>(load_config_by_dir(fs::current_path())) );

    const fs::path config_path = fs::current_path() / "poac.yml";
    std::ofstream(config_path.string());

    BOOST_CHECK( static_cast<bool>(load_config_by_dir(fs::current_path())) );

    fs::remove(config_path);
}

// YAML::Node load_config_by_dir_with_throw(const boost::filesystem::path& base)
BOOST_AUTO_TEST_CASE( poac_io_yaml_load_config_by_dir_with_throw_test )
{
    namespace fs = boost::filesystem;
    using poac::io::yaml::load_config_by_dir_with_throw;

    BOOST_CHECK_THROW(
            load_config_by_dir_with_throw(fs::current_path()),
            poac::core::except::error
    );

    const fs::path config_path = fs::current_path() / "poac.yml";
    std::ofstream(config_path.string());

    BOOST_CHECK_NO_THROW( load_config_by_dir_with_throw(fs::current_path()) );

    fs::remove(config_path);
}

// std::string get_timestamp()
BOOST_AUTO_TEST_CASE( poac_io_yaml_get_timestamp_test )
{
    namespace fs = boost::filesystem;
    using poac::io::yaml::get_timestamp;

    BOOST_CHECK_THROW(
            get_timestamp(),
            poac::core::except::error
    );

    const fs::path config_path = fs::current_path() / "poac.yml";
    std::ofstream(config_path.string());

    BOOST_CHECK_NO_THROW( get_timestamp() );

    fs::remove(config_path);
}
