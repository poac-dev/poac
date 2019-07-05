#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/util/semver/parser/parser.hpp>

BOOST_AUTO_TEST_CASE( semver_parser_parser_parse_empty_test )
{
    using semver::parser::Parser;

    Parser parser("");
    BOOST_CHECK_THROW(
            parser.version(),
            std::bad_optional_access
    );
    // empty string incorrectly considered a valid parse
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_parse_blank_test )
{
    using semver::parser::Parser;

    Parser parser("  ");
    BOOST_CHECK_THROW(
            parser.version(),
            std::bad_optional_access
    );
    // blank string incorrectly considered a valid parse
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_parse_no_minor_patch_test )
{
    using semver::parser::Parser;

    Parser parser("1");
    BOOST_CHECK_THROW(
            parser.version(),
            std::bad_optional_access
    );
    // 1 incorrectly considered a valid parse
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_parse_no_patch_test )
{
    using semver::parser::Parser;

    Parser parser("1.2");
    BOOST_CHECK_THROW(
            parser.version(),
            std::bad_optional_access
    );
    // 1.2 incorrectly considered a valid parse
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_parse_empty_pre_test )
{
    using semver::parser::Parser;

    Parser parser("1.2.3-");
    BOOST_CHECK_THROW(
            parser.version(),
            std::bad_optional_access
    );
    // 1.2.3- incorrectly considered a valid parse
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_parse_letters_test )
{
    using semver::parser::Parser;

    Parser parser("a.b.c");
    BOOST_CHECK_THROW(
            parser.version(),
            std::bad_optional_access
    );
    // a.b.c incorrectly considered a valid parse
}

//BOOST_AUTO_TEST_CASE( semver_parser_parser_parse_with_letters_test )
//{
//    using semver::parser::Parser;
//
//    Parser parser("1.2.3 a.b.c");
//    BOOST_CHECK_THROW(
//            parser.version(),
//            std::bad_optional_access
//    );
//    // 1.2.3 a.b.c incorrectly considered a valid parse
//}

BOOST_AUTO_TEST_CASE( semver_parser_parser_basic_version_test )
{
    using semver::parser::Parser;

    Parser parser("1.2.3");
    const auto parsed = parser.version();
    BOOST_CHECK( parsed.major == 1 );
    BOOST_CHECK( parsed.minor == 2 );
    BOOST_CHECK( parsed.patch == 3 );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_trims_input_test )
{
    using semver::parser::Parser;

    Parser parser("  1.2.3  ");
    const auto parsed = parser.version();
    BOOST_CHECK( parsed.major == 1 );
    BOOST_CHECK( parsed.minor == 2 );
    BOOST_CHECK( parsed.patch == 3 );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_basic_prerelease_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("1.2.3-pre");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_pre = {
            Identifier(Identifier::AlphaNumeric, "pre")
    };
    BOOST_CHECK( expected_pre == parsed.pre );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_prerelease_alphanumeric_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("1.2.3-alpha1");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_pre = {
            Identifier(Identifier::AlphaNumeric, "alpha1")
    };
    BOOST_CHECK( expected_pre == parsed.pre );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_prerelease_zero_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("1.2.3-pre.0");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_pre = {
            Identifier(Identifier::AlphaNumeric, "pre"),
            Identifier(Identifier::Numeric, 0)
    };
    BOOST_CHECK( expected_pre == parsed.pre );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_basic_build_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("1.2.3+build");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_build = {
            Identifier(Identifier::AlphaNumeric, "build")
    };
    BOOST_CHECK( expected_build == parsed.build );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_build_alphanumeric_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("1.2.3+build5");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_build = {
            Identifier(Identifier::AlphaNumeric, "build5")
    };
    BOOST_CHECK( expected_build == parsed.build );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_pre_and_build_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("1.2.3-alpha1+build5");
    const auto parsed = parser.version();

    const std::vector<Identifier> expected_pre = {
            Identifier(Identifier::AlphaNumeric, "alpha1")
    };
    BOOST_CHECK( expected_pre == parsed.pre );

    const std::vector<Identifier> expected_build = {
            Identifier(Identifier::AlphaNumeric, "build5")
    };
    BOOST_CHECK( expected_build == parsed.build );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_complex_metadata_01_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("1.2.3-1.alpha1.9+build5.7.3aedf  ");
    const auto parsed = parser.version();

    const std::vector<Identifier> expected_pre = {
            Identifier(Identifier::Numeric, 1),
            Identifier(Identifier::AlphaNumeric, "alpha1"),
            Identifier(Identifier::Numeric, 9)
    };
    BOOST_CHECK( expected_pre == parsed.pre );

    const std::vector<Identifier> expected_build = {
            Identifier(Identifier::AlphaNumeric, "build5"),
            Identifier(Identifier::Numeric, 7),
            Identifier(Identifier::AlphaNumeric, "3aedf")
    };
    BOOST_CHECK( expected_build == parsed.build );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_complex_metadata_02_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("0.4.0-beta.1+0851523");
    const auto parsed = parser.version();

    const std::vector<Identifier> expected_pre = {
            Identifier(Identifier::AlphaNumeric, "beta"),
            Identifier(Identifier::Numeric, 1)
    };
    BOOST_CHECK( expected_pre == parsed.pre );

    const std::vector<Identifier> expected_build = {
            Identifier(Identifier::AlphaNumeric, "0851523")
    };
    BOOST_CHECK( expected_build == parsed.build );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_parse_regression_01_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("0.0.0-WIP");
    const auto parsed = parser.version();

    BOOST_CHECK( parsed.major == 0 );
    BOOST_CHECK( parsed.minor == 0 );
    BOOST_CHECK( parsed.patch == 0 );

    const std::vector<Identifier> expected_pre = {
            Identifier(Identifier::AlphaNumeric, "WIP")
    };
    BOOST_CHECK( expected_pre == parsed.pre );
}
