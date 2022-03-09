#include <boost/ut.hpp>

#include <poac/util/semver/version.hpp>
#include <poac/util/semver/comparison.hpp>

int main() {
    using namespace std::literals::string_literals;
    using namespace boost::ut;

    using semver::Version;

    "test semver_eq"_test = [] {
        expect(Version("1.2.3") == "1.2.3");
        expect(Version("1.2.3-alpha") == "1.2.3-alpha");
        expect(Version("1.2.3+2013") == "1.2.3+2014");
        expect(Version("1.2.3-alpha+2013") == "1.2.3-alpha+2014");
        expect(Version("1.0.0-beta+exp.sha.5114f85") == "1.0.0-beta");
    };

    "test semver_lt"_test = [] {
        expect(Version("1.2.3") < "1.2.4");
        expect(Version("1.2.3") < "1.3.3");
        expect(Version("1.2.3") < "2.2.3");
        expect(Version("1.2.3") < "1.3.3");
        expect(!(Version("9.1.1") < "1.9.9"));
        expect(Version("1.2.3-alpha") < "1.2.3");
        expect(Version("1.2.3-alpha") < "1.2.3-beta");
        expect(Version("1.2.3-beta") < "1.2.3-pre");
        expect(Version("1.2.3-beta+build") < "1.2.3-pre+build");
        expect(Version("1.0.0-alpha") < "1.0.0-alpha.1");
        expect(Version("1.0.0-alpha.1") < "1.0.0-alpha.beta");
        expect(Version("1.0.0-alpha.beta") < "1.0.0-beta");
        expect(Version("1.0.0-beta") < "1.0.0-beta.2");
        expect(Version("1.0.0-beta.2") < "1.0.0-beta.11");
        expect(Version("1.0.0-beta.11") < "1.0.0-rc.1");
        expect(Version("1.0.0-rc.1") < "1.0.0");
    };

    "test semver_lte"_test = [] {
        expect(Version("1.2.3") <= "1.2.3");
        expect(Version("1.2.3-alpha") <= "1.2.3-alpha");
        expect(Version("1.2.3+2013") <= "1.2.3+2014");
        expect(Version("1.2.3-alpha+2013") <= "1.2.3-alpha+2014");
        expect(Version("1.0.0-beta+exp.sha.5114f85") <= "1.0.0-beta");

        expect(Version("1.2.3") <= "1.2.4");
        expect(Version("1.2.3") <= "1.3.3");
        expect(Version("1.2.3") <= "2.2.3");
        expect(Version("1.2.3") <= "1.3.3");
        expect(!(Version("9.1.1") <= "1.9.9"));
        expect(Version("1.2.3-alpha") <= "1.2.3");
        expect(Version("1.2.3-alpha") <= "1.2.3-beta");
        expect(Version("1.2.3-beta") <= "1.2.3-pre");
        expect(Version("1.2.3-beta+build") <= "1.2.3-pre+build");
        expect(Version("1.0.0-alpha") <= "1.0.0-alpha.1");
        expect(Version("1.0.0-alpha.1") <= "1.0.0-alpha.beta");
        expect(Version("1.0.0-alpha.beta") <= "1.0.0-beta");
        expect(Version("1.0.0-beta") <= "1.0.0-beta.2");
        expect(Version("1.0.0-beta.2") <= "1.0.0-beta.11");
        expect(Version("1.0.0-beta.11") <= "1.0.0-rc.1");
        expect(Version("1.0.0-rc.1") <= "1.0.0");
    };

    "test semver_gt"_test = [] {
        expect(Version("1.2.4") > "1.2.3");
        expect(Version("1.3.3") > "1.2.3");
        expect(Version("2.2.3") > "1.2.3");
        expect(Version("1.3.3") > "1.2.3");
        expect(!(Version("1.9.9") > "9.1.1"));
        expect(Version("1.2.3") > "1.2.3-alpha");
        expect(Version("1.2.3-beta") > "1.2.3-alpha");
        expect(Version("1.2.3-pre") > "1.2.3-beta");
        expect(Version("1.2.3-pre+build") > "1.2.3-beta+build");
        expect(Version("1.0.0-alpha.1") > "1.0.0-alpha");
        expect(Version("1.0.0-alpha.beta") > "1.0.0-alpha.1");
        expect(Version("1.0.0-beta") > "1.0.0-alpha.beta");
        expect(Version("1.0.0-beta.2") > "1.0.0-beta");
        expect(Version("1.0.0-beta.11") > "1.0.0-beta.2");
        expect(Version("1.0.0-rc.1") > "1.0.0-beta.11");
        expect(Version("1.0.0") > "1.0.0-rc.1");
    };

    "test semver_gte"_test = [] {
        expect(Version("1.2.3") >= "1.2.3");
        expect(Version("1.2.3-alpha") >= "1.2.3-alpha");
        expect(Version("1.2.3+2013") >= "1.2.3+2014");
        expect(Version("1.2.3-alpha+2013") >= "1.2.3-alpha+2014");
        expect(Version("1.0.0-beta+exp.sha.5114f85") >= "1.0.0-beta");

        expect(Version("1.2.4") >= "1.2.3");
        expect(Version("1.3.3") >= "1.2.3");
        expect(Version("2.2.3") >= "1.2.3");
        expect(Version("1.3.3") >= "1.2.3");
        expect(!(Version("1.9.9") >= "9.1.1"));
        expect(Version("1.2.3") >= "1.2.3-alpha");
        expect(Version("1.2.3-beta") >= "1.2.3-alpha");
        expect(Version("1.2.3-pre") >= "1.2.3-beta");
        expect(Version("1.2.3-pre+build") >= "1.2.3-beta+build");
        expect(Version("1.0.0-alpha.1") >= "1.0.0-alpha");
        expect(Version("1.0.0-alpha.beta") >= "1.0.0-alpha.1");
        expect(Version("1.0.0-beta") >= "1.0.0-alpha.beta");
        expect(Version("1.0.0-beta.2") >= "1.0.0-beta");
        expect(Version("1.0.0-beta.11") >= "1.0.0-beta.2");
        expect(Version("1.0.0-rc.1") >= "1.0.0-beta.11");
        expect(Version("1.0.0") >= "1.0.0-rc.1");
    };
}

// TODO: regex, Version constructor, get_version(), get_full(), Version exceptions
