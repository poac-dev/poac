#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <poac/core/builder/ninja_syntax.hpp>

#include <string>
#include <sstream>

//static const std::string LONG_WORD = std::string(10, 'a');
//static const std::string LONGWORDWITHSPACES = std::string(5, 'a') + "$ " + std::string(5, 'a');
//static const std::string INDENT = "    ";

BOOST_AUTO_TEST_CASE( poac_core_builder_ninja_syntax__test_build__test_variables_dict )
{
    namespace ninja_syntax = poac::core::builder::ninja_syntax;

    std::ostringstream ss;
    ninja_syntax::writer writer{ std::move(ss) };
    writer.build(
        std::vector<std::filesystem::path>{"out"},
        "cc",
        ninja_syntax::build_set_t{
            .inputs = "in",
            .variables = std::unordered_map<std::string, std::string>{
                {"name", "value"}
            }
        }
    );

    std::string expected =
        "build out: cc in\n"
        "  name = value\n";
    BOOST_CHECK(expected == writer.get_value());
}

BOOST_AUTO_TEST_CASE( poac_core_builder_ninja_syntax__test_build__test_implicit_outputs )
{
    namespace ninja_syntax = poac::core::builder::ninja_syntax;

    std::ostringstream ss;
    ninja_syntax::writer writer{ std::move(ss) };
    writer.build(
        std::vector<std::filesystem::path>{"o"},
        "cc",
        ninja_syntax::build_set_t{
            .inputs = "i",
            .implicit_outputs = "io",
        }
    );

    std::string expected = "build o | io: cc i\n";
    BOOST_CHECK(expected == writer.get_value());
}
