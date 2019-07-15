#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <string_view>
#include <fstream>

#include <poac/core/except.hpp>
#include <poac/io/yaml.hpp>
#include <poac/opts/publish.hpp>
#include <poac/config.hpp>

// std::optional<std::string> get_license(const std::string& full_name, const std::string& version)
BOOST_AUTO_TEST_CASE( poac_opts_publish_get_license_test )
{
    using poac::opts::publish::get_license;
    BOOST_CHECK( get_license("poacpm/poac", "0.2.1").value() == "GNU General Public License v3.0" );
    BOOST_CHECK( get_license("poacpm/api.poac.pm", "master").value() == "MIT License" );
}

// std::uint16_t get_cpp_version()
BOOST_AUTO_TEST_CASE( poac_opts_publish_get_cpp_version_test )
{
    namespace fs = boost::filesystem;
    using poac::opts::publish::get_cpp_version;

    const fs::path config_path = fs::current_path() / "poac.yml";
    {
        std::ofstream ofs(config_path.string());
        ofs << "cpp_version: 17";
    }

    BOOST_CHECK( get_cpp_version(poac::io::yaml::load()) == 17 );
    fs::remove(config_path);
}

// std::optional<std::string> get_description(const std::string& full_name)
BOOST_AUTO_TEST_CASE( poac_opts_publish_get_description_test )
{
    using poac::opts::publish::get_description;
    BOOST_CHECK( get_description("poacpm/poac").value() == "Package manager for C++" );
    BOOST_CHECK( !get_description("matken11235/to_TFRecord").has_value() );
}

// core::resolver::semver::Version get_version(const std::string& full_name)
BOOST_AUTO_TEST_CASE( poac_opts_publish_get_version_test )
{
    using poac::opts::publish::get_version;
    BOOST_CHECK( get_version() == POAC_VERSION );
}

// std::optional<std::string_view> extract_str(std::string_view target, std::string_view prefix, std::string_view suffix)
BOOST_AUTO_TEST_CASE( poac_opts_publish_extract_str_test )
{
    using poac::opts::publish::extract_str;

    {
        std::string_view target = "https://github.com/poacpm/poac.git";
        BOOST_CHECK( extract_str(target, "https://github.com/", ".git").value() == "poacpm/poac" );
    }
    {
        std::string_view target = "git@github.com:poacpm/poac.git";
        BOOST_CHECK( !extract_str(target, "https://github.com/", ".git").has_value() );
    }
    {
        std::string_view target = "git@github.com:poacpm/poac.git";
        BOOST_CHECK( extract_str(target, "git@github.com:", ".git").value() == "poacpm/poac" );
    }
}

// std::string_view extract_full_name(std::string_view repository)
BOOST_AUTO_TEST_CASE( poac_opts_publish_extract_full_name_test )
{
    using poac::opts::publish::extract_full_name;

    {
        std::string_view repo = "https://github.com/poacpm/poac.git";
        BOOST_CHECK( extract_full_name(repo) == "poacpm/poac" );
    }
    {
        std::string_view repo = "github.com/poacpm/poac.git";
        BOOST_CHECK_THROW(
                extract_full_name(repo),
                poac::core::except::error
        );
    }
    {
        std::string_view repo = "git@github.com:poacpm/poac.git";
        BOOST_CHECK( extract_full_name(repo) == "poacpm/poac" );
    }
}

// std::string get_name()
BOOST_AUTO_TEST_CASE( poac_opts_publish_get_name_test )
{
    using poac::opts::publish::get_name;
    BOOST_CHECK( get_name() == "poacpm/poac" );
}
