#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

#include <poac/io/config.hpp>
#include <poac/io/filesystem.hpp>
#include <poac/core/except.hpp>

#include "support/macros.hpp"
#include "support/test_ofstream.hpp"

// [[noreturn]] inline void rethrow_bad_cast(const std::string& what)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_rethrow_bad_cast_test )
{
    using poac::io::config::detail::rethrow_bad_cast;
    using poac::io::config::general_error;

    BOOST_CHECK_THROW_MSG(
        rethrow_bad_cast(
            "[error] toml::value bad_cast to string\n"
            " --> poac.toml\n"
            " 2 | name = []\n"
            "   |        ~~ the actual type is array"
        ),
        general_error,
        "[error] value type should be string\n"
        " --> poac.toml\n"
        " 2 | name = []\n"
        "   |        ~~ the actual type is array"
    );
}

// [[noreturn]] void rethrow_cfg_exception(const util::cfg::exception& e, const toml::value& v)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_rethrow_cfg_exception_test )
{
    using poac::io::config::detail::rethrow_cfg_exception;
    using poac::io::config::general_error;
    using toml::toml_literals::operator""_toml;

    support::test_ofstream ofs("poac.toml");
    ofs << "[target.'cfg(os = \"linux\"'.profile]";
    ofs.close();

    const auto target = toml::find<toml::table>(toml::parse("poac.toml"), "target");
    for (const auto& [key, value] : target) {
        try {
            poac::util::cfg::parse(key);
        } catch (const poac::util::cfg::exception& e) {
            BOOST_CHECK( std::string(e.what()) ==
                "cfg syntax error\n"
                "cfg(os = \"linux\"\n"
                "                ^ expected ')', but cfg expression ended"
            );
            BOOST_CHECK_THROW_MSG(
                rethrow_cfg_exception(e, target.at(key)),
                general_error,
                "[error] cfg syntax error\n"
                " --> poac.toml\n"
                "1 | [target.'cfg(os = \"linux\"'.profile]\n"
                "  |                          ^ expected ')', but cfg expression ended"
            );
        }
    }
}

// [[noreturn]] inline void rethrow_cfg_expr_error(const util::cfg::expression_error& e, const toml::value& v)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_rethrow_cfg_expr_error_test )
{
    using poac::io::config::detail::rethrow_cfg_expr_error;
    using poac::io::config::general_error;

    support::test_ofstream ofs("poac.toml");
    ofs << "[target.'   '.profile]";
    ofs.close();

    const auto target = toml::find<toml::table>(toml::parse("poac.toml"), "target");
    for (const auto& [key, value] : target) {
        try {
            poac::util::cfg::parse(key);
        } catch (const poac::util::cfg::expression_error& e) {
            BOOST_CHECK( std::string(e.what()) == "expected start of a cfg expression" );
            BOOST_CHECK_THROW_MSG(
                rethrow_cfg_expr_error(e, target.at(key)),
                general_error,
                "[error] cfg expression error\n"
                " --> poac.toml\n"
                " 1 | [target.'   '.profile]\n"
                "   | ~~~~~~~~~~~~~~~~~~~~~~ expected start of a cfg expression"
            );
        }
    }
}

// find_force(const toml::basic_value<C, M, V>& v, const toml::key& key)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_find_force_test )
{
    using poac::io::config::detail::find_force;
    using poac::io::config::general_error;
    using toml::toml_literals::operator""_toml;

    BOOST_CHECK( find_force<std::string>("name = \"poac\""_toml, "name") == "poac" );
    BOOST_CHECK_THROW_MSG(
        find_force<int>("name = \"poac\""_toml, "name"),
        general_error,
        "[error] value type should be integer\n"
        " --> TOML literal encoded in a C++ code\n"
        " 1 | name = \"poac\"\n"
        "   |        ~~~~~~ the actual type is string"
    );
    BOOST_CHECK_THROW_MSG(
        find_force<std::string>("name = \"poac\""_toml, "foo"),
        std::out_of_range,
        "[error] key \"foo\" not found\n"
        " --> TOML literal encoded in a C++ code\n"
        " 1 | name = \"poac\"\n"
        "   | ~~~~~~~~~~~~~ in this table"
    );
}

// find_force_opt(const toml::basic_value<C, M, V>& v, const toml::key& key)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_find_force_opt_test )
{
    using poac::io::config::detail::find_force_opt;
    using poac::io::config::general_error;
    using toml::toml_literals::operator""_toml;

    BOOST_CHECK( find_force_opt<std::string>("name = \"poac\""_toml, "name") == "poac" );
    BOOST_CHECK_THROW_MSG(
        find_force_opt<int>("name = \"poac\""_toml, "name"),
        general_error,
        "[error] value type should be integer\n"
        " --> TOML literal encoded in a C++ code\n"
        " 1 | name = \"poac\"\n"
        "   |        ~~~~~~ the actual type is string"
    );
    BOOST_CHECK( !find_force_opt<std::string>("name = \"poac\""_toml, "unknown").has_value() );
}

// find_opt(const toml::basic_value<C, M, V>& v, const toml::key& key)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_find_opt_test )
{
    using poac::io::config::detail::find_opt;
    using toml::toml_literals::operator""_toml;

    BOOST_CHECK( find_opt<std::string>("name = \"poac\""_toml, "name") == "poac" );
    BOOST_CHECK( !find_opt<int>("name = \"poac\""_toml, "name").has_value() );
    BOOST_CHECK( !find_opt<std::string>("name = \"poac\""_toml, "unknown").has_value() );
}

// inline std::string to_toml_array_string(std::vector<T>&& v)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_to_toml_array_string_test )
{
    using poac::io::config::detail::to_toml_array_string;

    {
        std::vector<std::string> s{"one", "two", "three"};
        std::string expect = "[\"one\", \"two\", \"three\"]";
        BOOST_CHECK( to_toml_array_string(std::move(s)) == expect );
    }
    {
        std::vector<int> i{1, 2, 3};
        std::string expect = "[1, 2, 3]";
        BOOST_CHECK( to_toml_array_string(std::move(i)) == expect );
    }
}

// find_enum(const toml::basic_value<C, M, V>& v, const toml::key& key, std::vector<T>&& pv)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_find_enum_test )
{
    using poac::io::config::detail::find_enum;
    using poac::io::config::general_error;
    using toml::toml_literals::operator""_toml;

    BOOST_CHECK( find_enum<std::string>("name = \"poac\""_toml, "name", {"poac"}) == "poac" );
    BOOST_CHECK_THROW_MSG(
        find_enum<std::string>("name = \"poac\""_toml, "name", {"foo"}),
        general_error,
        "[error] value should be any of [\"foo\"]\n"
        " --> TOML literal encoded in a C++ code\n"
        " 1 | name = \"poac\"\n"
        "   |        ~~~~~~ one of the above listed is required"
    );
    BOOST_CHECK_THROW_MSG(
        find_enum<int>("name = \"poac\""_toml, "name", {2}),
        general_error,
        "[error] value type should be integer\n"
        " --> TOML literal encoded in a C++ code\n"
        " 1 | name = \"poac\"\n"
        "   |        ~~~~~~ the actual type is string"
    );
    BOOST_CHECK_THROW_MSG(
        find_enum<std::string>("name = \"poac\""_toml, "foo", {"poac"}),
        std::out_of_range,
        "[error] key \"foo\" not found\n"
        " --> TOML literal encoded in a C++ code\n"
        " 1 | name = \"poac\"\n"
        "   | ~~~~~~~~~~~~~ in this table"
    );
}

// find_enum_opt(const toml::basic_value<C, M, V>& v, const toml::key& key, std::vector<T>&& pv)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_find_enum_opt_test )
{
    using poac::io::config::detail::find_enum_opt;
    using poac::io::config::general_error;
    using toml::toml_literals::operator""_toml;

    BOOST_CHECK( find_enum_opt<std::string>("name = \"poac\""_toml, "name", {"poac"}) == "poac" );
    BOOST_CHECK_THROW_MSG(
        find_enum_opt<std::string>("name = \"poac\""_toml, "name", {"foo"}),
        general_error,
        "[error] value should be any of [\"foo\"]\n"
        " --> TOML literal encoded in a C++ code\n"
        " 1 | name = \"poac\"\n"
        "   |        ~~~~~~ one of the above listed is required"
    );
    BOOST_CHECK_THROW_MSG(
        find_enum_opt<int>("name = \"poac\""_toml, "name", {2}),
        general_error,
        "[error] value type should be integer\n"
        " --> TOML literal encoded in a C++ code\n"
        " 1 | name = \"poac\"\n"
        "   |        ~~~~~~ the actual type is string"
    );
    BOOST_CHECK( !find_enum_opt<std::string>("name = \"poac\""_toml, "unknown", {"poac"}).has_value() );
}

// merge(Field& f, const Field& f2)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_merge_test )
{
    namespace detail = poac::io::config::detail;
    using toml::toml_literals::operator""_toml;

    {
        std::vector<int> f{ 1, 2, 3 };
        const std::vector<int> f2{ 4, 5 };
        detail::merge(f, f2);
        const std::vector<int> expect{ 1, 2, 3, 4, 5 };
        BOOST_CHECK( f == expect );
    }
    {
        std::unordered_map<int, int> f{{1, 2}, {2, 3}, {3, 4}};
        const std::unordered_map<int, int> f2{{4, 5}, {5, 6}};
        detail::merge(f, f2);
        const std::unordered_map<int, int> expect{{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}};
        BOOST_CHECK( f == expect );
    }
    {
        struct foo {
            std::vector<int> value;
            inline void merge(const foo& f2) {
                detail::merge(value, f2.value);
            }
        };
        foo f{{ 1, 2, 3 }};
        const foo f2{{ 4, 5 }};
        detail::merge(f, f2);
        const foo expect{{ 1, 2, 3, 4, 5 }};
        BOOST_CHECK( f.value == expect.value );
    }
    {
        std::optional<std::vector<int>> f = std::nullopt;
        const std::optional<std::vector<int>> f2 = std::nullopt;
        detail::merge(f, f2);
        BOOST_CHECK( !f.has_value() );
    }
    {
        std::optional<std::vector<int>> f = std::nullopt;
        const std::optional<std::vector<int>> f2 = std::vector<int>{ 4, 5 };
        detail::merge(f, f2);
        BOOST_CHECK( f.has_value() );
        const std::vector<int> expect{ 4, 5 };
        BOOST_CHECK( f == expect );
    }
    {
        std::optional<std::vector<int>> f = std::vector<int>{ 1, 2, 3 };
        const std::optional<std::vector<int>> f2 = std::vector<int>{ 4, 5 };
        detail::merge(f, f2);
        BOOST_CHECK( f.has_value() );
        const std::vector<int> expect{ 1, 2, 3, 4, 5 };
        BOOST_CHECK( f == expect );
    }
    {
        std::optional<std::vector<int>> f = std::vector<int>{ 1, 2, 3 };
        detail::merge(f, "list = [4, 5]"_toml, "list");
        const std::vector<int> expect{ 1, 2, 3, 4, 5 };
        BOOST_CHECK( f == expect );
    }
}

// std::optional<std::string> validate_config(const io::filesystem::path& base = filesystem::current)
BOOST_AUTO_TEST_CASE( poac_io_config_detail_validate_config_test )
{
    using poac::io::config::detail::validate_config;
    using poac::io::filesystem::operator""_path;

    BOOST_CHECK( !validate_config().has_value() );
    {
        support::test_ofstream ofs("poac.toml");
        BOOST_CHECK( validate_config().has_value() );
    }
    BOOST_CHECK( !validate_config("test").has_value() );
    {
        poac::io::filesystem::create_directory("test");
        support::test_ofstream ofs("test" / "poac.toml"_path);
        BOOST_CHECK( validate_config("test").has_value() );
    }
    poac::io::filesystem::remove("test");
}

// std::optional<C> load_toml(const io::filesystem::path& base, const std::string& fname)
BOOST_AUTO_TEST_CASE( poac_io_config_load_toml_test )
{
    using poac::io::config::load_toml;
    using poac::io::config::Config;
    using poac::io::filesystem::operator""_path;

    BOOST_CHECK( !load_toml<Config>(poac::io::filesystem::current, "poac.toml").has_value() );
    {
        support::test_ofstream ofs("poac.toml");
        ofs << "[package]\n"
               "name = \"foo\"\n"
               "version = \"1.0.0\"\n";
        ofs.close();
        BOOST_CHECK( load_toml<Config>(poac::io::filesystem::current, "poac.toml").has_value() );
    }
    BOOST_CHECK( !load_toml<Config>("test", "poac.toml").has_value() );
    {
        poac::io::filesystem::create_directory("test");
        support::test_ofstream ofs("test" / "poac.toml"_path);
        ofs << "[package]\n"
               "name = \"foo\"\n"
               "version = \"1.0.0\"\n";
        ofs.close();
        BOOST_CHECK( load_toml<Config>("test", "poac.toml").has_value() );
    }
    poac::io::filesystem::remove("test");
}

// std::optional<Config> load(const io::filesystem::path& base = filesystem::current)
BOOST_AUTO_TEST_CASE( poac_io_config_load_test )
{
    using poac::io::config::load;
    using poac::io::filesystem::operator""_path;

    BOOST_CHECK( !load().has_value() );
    {
        support::test_ofstream ofs("poac.toml");
        ofs << "[package]\n"
               "name = \"foo\"\n"
               "version = \"1.0.0\"\n";
        ofs.close();
        BOOST_CHECK( load().has_value() );
    }
    BOOST_CHECK( !load("test").has_value() );
    {
        poac::io::filesystem::create_directory("test");
        support::test_ofstream ofs("test" / "poac.toml"_path);
        ofs << "[package]\n"
               "name = \"foo\"\n"
               "version = \"1.0.0\"\n";
        ofs.close();
        BOOST_CHECK( load("test").has_value() );
    }
    poac::io::filesystem::remove("test");
}

// std::string get_timestamp()
BOOST_AUTO_TEST_CASE( poac_io_config_get_timestamp_test )
{
    using poac::io::config::get_timestamp;

    BOOST_CHECK_THROW( get_timestamp(), poac::core::except::error );
    support::test_ofstream ofs("poac.toml");
    BOOST_CHECK( !get_timestamp().empty() );
}

BOOST_AUTO_TEST_CASE( poac_io_config_manifest_enum_test )
{
    using poac::io::config::Config;
    using poac::io::config::general_error;
    using toml::toml_literals::operator""_toml;

    BOOST_CHECK_THROW_MSG(
        toml::get<Config>(
            "[package]\n"
            "name = \"foo\"\n"
            "version = \"1.0.0\"\n"
            "cpp = 12\n"_toml
        ),
        general_error,
        "[error] value should be any of [98, 3, 11, 14, 17, 20]\n"
        " --> TOML literal encoded in a C++ code\n"
        " 4 | cpp = 12\n"
        "   |       ~~ one of the above listed is required"
    );
    BOOST_CHECK_THROW_MSG(
        toml::get<Config>(
            "[package]\n"
            "name = \"foo\"\n"
            "version = \"1.0.0\"\n"
            "cpp = 17\n"
            "[profile.dev]\n"
            "opt-level = \"w\""_toml
        ),
        general_error,
        "[error] value should be any of [\"0\", \"1\", \"2\", \"3\", \"g\", \"s\"]\n"
        " --> TOML literal encoded in a C++ code\n"
        " 6 | opt-level = \"w\"\n"
        "   |             ~~~ one of the above listed is required"
    );
    BOOST_CHECK_NO_THROW(
        toml::get<Config>(
            "[package]\n"
            "name = \"foo\"\n"
            "version = \"1.0.0\"\n"
            "cpp = 17\n"
            "[profile.dev]\n"
            "opt-level = \"g\""_toml
        )
    );
}
