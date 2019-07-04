#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <poac/util/semver/parser/parser.hpp>

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
    const std::vector<Identifier> expected_pre = {Identifier(Identifier::AlphaNumeric, "pre")};
    BOOST_CHECK( expected_pre == parsed.pre );
}

BOOST_AUTO_TEST_CASE( semver_parser_parser_prerelease_alphanumeric_test )
{
    using semver::parser::Parser;
    using semver::parser::Identifier;

    Parser parser("1.2.3-alpha1");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_pre = {Identifier(Identifier::AlphaNumeric, "alpha1")};
    BOOST_CHECK( expected_pre == parsed.pre );
}
