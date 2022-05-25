#include <boost/ut.hpp>
#include <poac/util/semver/parser.hpp>

int
main() {
  using namespace std::literals::string_literals;
  using namespace boost::ut;

  using semver::Parser;

  "test parse empty"_test = [] {
    Parser parser("");
    expect(throws<std::bad_optional_access>([&] { parser.version(); }))
        << "empty string incorrectly considered a valid parse";
  };

  "test parse blank"_test = [] {
    Parser parser("  ");
    expect(throws<std::bad_optional_access>([&] { parser.version(); }))
        << "blank string incorrectly considered a valid parse";
  };

  "test parse no minor patch test"_test = [] {
    Parser parser("1");
    expect(throws<std::bad_optional_access>([&] { parser.version(); }))
        << "1 incorrectly considered a valid parse";
  };

  "test parse no patch test"_test = [] {
    Parser parser("1.2");
    expect(throws<std::bad_optional_access>([&] { parser.version(); }))
        << "1.2 incorrectly considered a valid parse";
  };

  "test parse empty pre"_test = [] {
    Parser parser("1.2.3-");
    expect(throws<std::bad_optional_access>([&] { parser.version(); }))
        << "1.2.3- incorrectly considered a valid parse";
  };

  "test parse letters"_test = [] {
    Parser parser("a.b.c");
    expect(throws<std::bad_optional_access>([&] { parser.version(); }))
        << "a.b.c incorrectly considered a valid parse";
  };

  "test parse with letters"_test = [] {
    Parser parser("1.2.3 a.b.c");
    expect(throws<semver::version_error>([&] { parser.version(); }))
        << "1.2.3 a.b.c incorrectly considered a valid parse";
  };

  "test parse basic version"_test = [] {
    Parser parser("1.2.3");
    const auto parsed = parser.version();
    expect(parsed.major == 1_i);
    expect(parsed.minor == 2_i);
    expect(parsed.patch == 3_i);
  };

  "test parser trims input"_test = [] {
    Parser parser("  1.2.3  ");
    const auto parsed = parser.version();
    expect(parsed.major == 1_i);
    expect(parsed.minor == 2_i);
    expect(parsed.patch == 3_i);
  };

  using semver::Identifier;

  "test parser basic prerelease"_test = [] {
    Parser parser("1.2.3-pre");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_pre = {
        Identifier(Identifier::AlphaNumeric, "pre")};
    expect(expected_pre == parsed.pre);
  };

  "test parser prerelease alphanumeric"_test = [] {
    Parser parser("1.2.3-alpha1");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_pre = {
        Identifier(Identifier::AlphaNumeric, "alpha1")};
    expect(expected_pre == parsed.pre);
  };

  "test parser prerelease zero"_test = [] {
    Parser parser("1.2.3-pre.0");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_pre = {
        Identifier(Identifier::AlphaNumeric, "pre"),
        Identifier(Identifier::Numeric, 0)};
    expect(expected_pre == parsed.pre);
  };

  "test parser basic build"_test = [] {
    Parser parser("1.2.3+build");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_build = {
        Identifier(Identifier::AlphaNumeric, "build")};
    expect(expected_build == parsed.build);
  };

  "test parser build alphanumeric"_test = [] {
    Parser parser("1.2.3+build5");
    const auto parsed = parser.version();
    const std::vector<Identifier> expected_build = {
        Identifier(Identifier::AlphaNumeric, "build5")};
    expect(expected_build == parsed.build);
  };

  "test parser pre & build"_test = [] {
    Parser parser("1.2.3-alpha1+build5");
    const auto parsed = parser.version();

    const std::vector<Identifier> expected_pre = {
        Identifier(Identifier::AlphaNumeric, "alpha1")};
    expect(expected_pre == parsed.pre);

    const std::vector<Identifier> expected_build = {
        Identifier(Identifier::AlphaNumeric, "build5")};
    expect(expected_build == parsed.build);
  };

  "test parser complex metadata 01"_test = [] {
    Parser parser("1.2.3-1.alpha1.9+build5.7.3aedf  ");
    const auto parsed = parser.version();

    const std::vector<Identifier> expected_pre = {
        Identifier(Identifier::Numeric, 1),
        Identifier(Identifier::AlphaNumeric, "alpha1"),
        Identifier(Identifier::Numeric, 9)};
    expect(expected_pre == parsed.pre);

    const std::vector<Identifier> expected_build = {
        Identifier(Identifier::AlphaNumeric, "build5"),
        Identifier(Identifier::Numeric, 7),
        Identifier(Identifier::AlphaNumeric, "3aedf")};
    expect(expected_build == parsed.build);
  };

  "test parser complex metadata 02"_test = [] {
    Parser parser("0.4.0-beta.1+0851523");
    const auto parsed = parser.version();

    const std::vector<Identifier> expected_pre = {
        Identifier(Identifier::AlphaNumeric, "beta"),
        Identifier(Identifier::Numeric, 1)};
    expect(expected_pre == parsed.pre);

    const std::vector<Identifier> expected_build = {
        Identifier(Identifier::AlphaNumeric, "0851523")};
    expect(expected_build == parsed.build);
  };

  "test parser parse regression 01"_test = [] {
    Parser parser("0.0.0-WIP");
    const auto parsed = parser.version();

    expect(parsed.major == 0_i);
    expect(parsed.minor == 0_i);
    expect(parsed.patch == 0_i);

    const std::vector<Identifier> expected_pre = {
        Identifier(Identifier::AlphaNumeric, "WIP")};
    expect(expected_pre == parsed.pre);
  };
}
