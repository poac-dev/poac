#include "../../../util/ut_helpers/throws_with_msg.hpp"

#include <boost/ut.hpp>
#include <poac/util/semver/exception.hpp>
#include <poac/util/semver/interval.hpp>

auto main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;
  using namespace boost::ut::spec;

  using semver::Interval;

  describe("test satisfies") = [] {
    it("test1") = [] {
      Interval const interval(">=1.66.0 and <1.70.0");
      expect(interval.satisfies("1.66.0"));
      expect(interval.satisfies("1.67.0"));
      expect(interval.satisfies("1.68.0"));
      expect(interval.satisfies("1.69.0"));
      expect(interval.satisfies("1.69.9"));
      expect(!interval.satisfies("1.70.0"));
    };

    it("test2") = [] {
      Interval const interval(">=1.0.0-alpha and <1.0.0");
      expect(interval.satisfies("1.0.0-alpha"));
      expect(interval.satisfies("1.0.0-alpha.1"));
      expect(interval.satisfies("1.0.0-alpha.beta"));
      expect(interval.satisfies("1.0.0-beta"));
      expect(interval.satisfies("1.0.0-beta.2"));
      expect(interval.satisfies("1.0.0-beta.11"));
      expect(interval.satisfies("1.0.0-rc.1"));
      expect(!interval.satisfies("1.0.0"));
    };
  };

  "test is_wasteful_comparison_operation"_test = [] {
    using semver::redundant_interval_error;
    using poac::test::ut_helpers::throws_with_msg;

    throws_with_msg<redundant_interval_error>(
        [] { Interval("<2.0.0 and <1.0.0"); },
        "`<2.0.0 and <1.0.0` is redundant expression.\n"
        "Did you mean <2.0.0 ?"
    );
    throws_with_msg<redundant_interval_error>(
        [] { Interval("<1.0.0 and <2.0.0"); },
        "`<1.0.0 and <2.0.0` is redundant expression.\n"
        "Did you mean <2.0.0 ?"
    );

    throws_with_msg<redundant_interval_error>(
        [] { Interval("<=2.0.0 and <=1.0.0"); },
        "`<=2.0.0 and <=1.0.0` is redundant expression.\n"
        "Did you mean <=2.0.0 ?"
    );
    throws_with_msg<redundant_interval_error>(
        [] { Interval("<2.0.0 and <=1.0.0"); },
        "`<2.0.0 and <=1.0.0` is redundant expression.\n"
        "Did you mean <2.0.0 ?"
    );
    throws_with_msg<redundant_interval_error>(
        [] { Interval("<=2.0.0 and <1.0.0"); },
        "`<=2.0.0 and <1.0.0` is redundant expression.\n"
        "Did you mean <=2.0.0 ?"
    );

    throws_with_msg<redundant_interval_error>(
        [] { Interval("<1.0.0 and <1.0.0-alpha"); },
        "`<1.0.0 and <1.0.0-alpha` is redundant expression.\n"
        "Did you mean <1.0.0 ?"
    );
    // TODO(ken-matsui)
    //        expect(throws<redundant_interval_error>([] {
    //            Interval("<1.0.0-alpha and <1.0.0");
    //        }));
    //        expect(eq(
    //            "`<1.0.0-alpha and <1.0.0` is redundant expression.\n"
    //            "Did you mean <1.0.0 ?"s,
    //            [] {
    //                try {
    //                    Interval("<1.0.0-alpha and <1.0.0");
    //                } catch (const redundant_interval_error& e) {
    //                    return std::string(e.what());
    //                }
    //                return std::string();
    //            }()
    //        ));

    throws_with_msg<redundant_interval_error>(
        [] { Interval("<1.0.0 and <1.0.0"); },
        "`<1.0.0 and <1.0.0` is redundant expression.\n"
        "Did you mean <1.0.0 ?"
    );

    throws_with_msg<redundant_interval_error>(
        [] { Interval(">2.0.0 and >1.0.0"); },
        "`>2.0.0 and >1.0.0` is redundant expression.\n"
        "Did you mean >1.0.0 ?"
    );
    throws_with_msg<redundant_interval_error>(
        [] { Interval(">=2.0.0 and >=1.0.0"); },
        "`>=2.0.0 and >=1.0.0` is redundant expression.\n"
        "Did you mean >=1.0.0 ?"
    );
    throws_with_msg<redundant_interval_error>(
        [] { Interval(">2.0.0 and >=1.0.0"); },
        "`>2.0.0 and >=1.0.0` is redundant expression.\n"
        "Did you mean >=1.0.0 ?"
    );
    throws_with_msg<redundant_interval_error>(
        [] { Interval(">=2.0.0 and >1.0.0"); },
        "`>=2.0.0 and >1.0.0` is redundant expression.\n"
        "Did you mean >1.0.0 ?"
    );

    throws_with_msg<redundant_interval_error>(
        [] { Interval(">1.0.0-alpha and >1.0.0"); },
        "`>1.0.0-alpha and >1.0.0` is redundant expression.\n"
        "Did you mean >1.0.0-alpha ?"
    );
    throws_with_msg<redundant_interval_error>(
        [] { Interval(">1.0.0 and >1.0.0"); },
        "`>1.0.0 and >1.0.0` is redundant expression.\n"
        "Did you mean >1.0.0 ?"
    );
  };

  "test is_bounded_interval"_test = [] {
    using semver::strange_interval_error;

    expect(throws<strange_interval_error>([] { Interval("<1.0.0 and >2.0.0"); })
    );
    // TODO(ken-matsui): test the throwed messages
    expect(throws<strange_interval_error>([] {
      Interval("<1.0.0-alpha and >1.0.0");
    }));
  };
}
