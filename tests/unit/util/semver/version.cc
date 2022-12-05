#include <boost/ut.hpp>
#include <poac/util/semver/comparison.hpp>
#include <poac/util/semver/token.hpp>

auto
main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;

  using semver::parse;

  "test semver_eq"_test = [] {
    expect(parse("1.2.3") == "1.2.3");
    expect(parse("1.2.3-alpha") == "1.2.3-alpha");
    expect(parse("1.2.3+2013") == "1.2.3+2014");
    expect(parse("1.2.3-alpha+2013") == "1.2.3-alpha+2014");
    expect(parse("1.0.0-beta+exp.sha.5114f85") == "1.0.0-beta");
  };

  "test semver_lt"_test = [] {
    expect(parse("1.2.3") < "1.2.4");
    expect(parse("1.2.3") < "1.3.3");
    expect(parse("1.2.3") < "2.2.3");
    expect(parse("1.2.3") < "1.3.3");
    expect(!(parse("9.1.1") < "1.9.9"));
    expect(parse("1.2.3-alpha") < "1.2.3");
    expect(parse("1.2.3-alpha") < "1.2.3-beta");
    expect(parse("1.2.3-beta") < "1.2.3-pre");
    expect(parse("1.2.3-beta+build") < "1.2.3-pre+build");
    expect(parse("1.0.0-alpha") < "1.0.0-alpha.1");
    expect(parse("1.0.0-alpha.1") < "1.0.0-alpha.beta");
    expect(parse("1.0.0-alpha.beta") < "1.0.0-beta");
    expect(parse("1.0.0-beta") < "1.0.0-beta.2");
    expect(parse("1.0.0-beta.2") < "1.0.0-beta.11");
    expect(parse("1.0.0-beta.11") < "1.0.0-rc.1");
    expect(parse("1.0.0-rc.1") < "1.0.0");
  };

  "test semver_lte"_test = [] {
    expect(parse("1.2.3") <= "1.2.3");
    expect(parse("1.2.3-alpha") <= "1.2.3-alpha");
    expect(parse("1.2.3+2013") <= "1.2.3+2014");
    expect(parse("1.2.3-alpha+2013") <= "1.2.3-alpha+2014");
    expect(parse("1.0.0-beta+exp.sha.5114f85") <= "1.0.0-beta");

    expect(parse("1.2.3") <= "1.2.4");
    expect(parse("1.2.3") <= "1.3.3");
    expect(parse("1.2.3") <= "2.2.3");
    expect(parse("1.2.3") <= "1.3.3");
    expect(!(parse("9.1.1") <= "1.9.9"));
    expect(parse("1.2.3-alpha") <= "1.2.3");
    expect(parse("1.2.3-alpha") <= "1.2.3-beta");
    expect(parse("1.2.3-beta") <= "1.2.3-pre");
    expect(parse("1.2.3-beta+build") <= "1.2.3-pre+build");
    expect(parse("1.0.0-alpha") <= "1.0.0-alpha.1");
    expect(parse("1.0.0-alpha.1") <= "1.0.0-alpha.beta");
    expect(parse("1.0.0-alpha.beta") <= "1.0.0-beta");
    expect(parse("1.0.0-beta") <= "1.0.0-beta.2");
    expect(parse("1.0.0-beta.2") <= "1.0.0-beta.11");
    expect(parse("1.0.0-beta.11") <= "1.0.0-rc.1");
    expect(parse("1.0.0-rc.1") <= "1.0.0");
  };

  "test semver_gt"_test = [] {
    expect(parse("1.2.4") > "1.2.3");
    expect(parse("1.3.3") > "1.2.3");
    expect(parse("2.2.3") > "1.2.3");
    expect(parse("1.3.3") > "1.2.3");
    expect(!(parse("1.9.9") > "9.1.1"));
    expect(parse("1.2.3") > "1.2.3-alpha");
    expect(parse("1.2.3-beta") > "1.2.3-alpha");
    expect(parse("1.2.3-pre") > "1.2.3-beta");
    expect(parse("1.2.3-pre+build") > "1.2.3-beta+build");
    expect(parse("1.0.0-alpha.1") > "1.0.0-alpha");
    expect(parse("1.0.0-alpha.beta") > "1.0.0-alpha.1");
    expect(parse("1.0.0-beta") > "1.0.0-alpha.beta");
    expect(parse("1.0.0-beta.2") > "1.0.0-beta");
    expect(parse("1.0.0-beta.11") > "1.0.0-beta.2");
    expect(parse("1.0.0-rc.1") > "1.0.0-beta.11");
    expect(parse("1.0.0") > "1.0.0-rc.1");
  };

  "test semver_gte"_test = [] {
    expect(parse("1.2.3") >= "1.2.3");
    expect(parse("1.2.3-alpha") >= "1.2.3-alpha");
    expect(parse("1.2.3+2013") >= "1.2.3+2014");
    expect(parse("1.2.3-alpha+2013") >= "1.2.3-alpha+2014");
    expect(parse("1.0.0-beta+exp.sha.5114f85") >= "1.0.0-beta");

    expect(parse("1.2.4") >= "1.2.3");
    expect(parse("1.3.3") >= "1.2.3");
    expect(parse("2.2.3") >= "1.2.3");
    expect(parse("1.3.3") >= "1.2.3");
    expect(!(parse("1.9.9") >= "9.1.1"));
    expect(parse("1.2.3") >= "1.2.3-alpha");
    expect(parse("1.2.3-beta") >= "1.2.3-alpha");
    expect(parse("1.2.3-pre") >= "1.2.3-beta");
    expect(parse("1.2.3-pre+build") >= "1.2.3-beta+build");
    expect(parse("1.0.0-alpha.1") >= "1.0.0-alpha");
    expect(parse("1.0.0-alpha.beta") >= "1.0.0-alpha.1");
    expect(parse("1.0.0-beta") >= "1.0.0-alpha.beta");
    expect(parse("1.0.0-beta.2") >= "1.0.0-beta");
    expect(parse("1.0.0-beta.11") >= "1.0.0-beta.2");
    expect(parse("1.0.0-rc.1") >= "1.0.0-beta.11");
    expect(parse("1.0.0") >= "1.0.0-rc.1");
  };

  "test get_version"_test = [] {
    expect(eq(parse("1.2.3").get_version(), "1.2.3"s));
    expect(eq(parse("1.2.3-alpha").get_version(), "1.2.3-alpha"s));
    expect(eq(parse("1.2.3+2013").get_version(), "1.2.3"s));
    expect(eq(parse("1.2.3-alpha+2013").get_version(), "1.2.3-alpha"s));
    expect(eq(parse("1.0.0-beta+exp.sha.5114f85").get_version(), "1.0.0-beta"s)
    );
  };

  "test get_full"_test = [] {
    expect(eq(parse("1.2.3").get_full(), "1.2.3"s));
    expect(eq(parse("1.2.3-alpha").get_full(), "1.2.3-alpha"s));
    expect(eq(parse("1.2.3+2013").get_full(), "1.2.3+2013"s));
    expect(eq(parse("1.2.3-alpha+2013").get_full(), "1.2.3-alpha+2013"s));
    expect(
        eq(parse("1.0.0-beta+exp.sha.5114f85").get_full(),
           "1.0.0-beta+exp.sha.5114f85"s)
    );
  };
}
