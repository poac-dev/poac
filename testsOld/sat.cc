#include <boost/ut.hpp>
#include <poac/core/resolver/sat.hpp>

auto
main() -> int {
  using namespace std::literals::string_literals;
  using namespace boost::ut;
  using namespace boost::ut::spec;
  using poac::core::resolver::sat::solve;

  describe("test solve => SAT") = [] {
    it("test1") = [] {
      const std::vector<std::vector<int>> clauses{ { 1, 2 },
                                                   { -1, 2 },
                                                   { -1, -2 } };
      const auto result = solve(clauses, 2);

      expect(result.is_ok());
      expect(eq(result.unwrap(), std::vector<int>({ -1, 2 })));
    };

    it("test2") = [] {
      const std::vector<std::vector<int>> clauses{
        { -1, -2, -3 }, { -2, -3, -4 }, { -2, -2, 3 }, { 2, 2, 2 }
      };
      const auto result = solve(clauses, 4);

      expect(result.is_ok());
      expect(eq(result.unwrap(), std::vector<int>({ -1, 2, 3, -4 })));
    };

    it("test3") = [] {
      const std::vector<std::vector<int>> clauses{
        { 1 }, { -2, 4, 5 }, { -2, 6 },  { -3, 5 }, { 2 },
        { 3 }, { 4, 5 },     { -4, -5 }, { 6 }
      };
      const auto result = solve(clauses, 6);

      expect(result.is_ok());
      expect(eq(result.unwrap(), std::vector<int>({ 1, 2, 3, -4, 5, 6 })));
    };

    it("test4") = [] {
      const std::vector<std::vector<int>> clauses{
        { 1, 2 }, { -2, 3 }, { -2, 4 }, { -4, 5 }, { 1 },
        { 2 },    { 3 },     { 4 },     { 5 }
      };
      const auto result = solve(clauses, 5);

      expect(result.is_ok());
      expect(eq(result.unwrap(), std::vector<int>({ 1, 2, 3, 4, 5 })));
    };

    it("test5") = [] {
      const std::vector<std::vector<int>> clauses{
        { 1, 2 }, { -3, 2 }, { -3, 4 }, { 3 }, { 4 }
      };
      const auto result = solve(clauses, 4);

      expect(result.is_ok());
      expect(eq(result.unwrap(), std::vector<int>({ 1, 2, 3, 4 })));
    };

    // FIXME: Maybe the SAT solver has a bug
    skip / it("test6") = [] {
      const std::vector<std::vector<int>> clauses{
        { 1 },         { -2, 6, 5, 4 }, { -3, 6, 4 },  { 2 },
        { 3 },         { 4, 5, 6 },     { -4, -5, 6 }, { -4, 5, -6 },
        { 4, -5, -6 }, { -4, -5, -6 },  { -4, 6 }
      };
      const auto result = solve(clauses, 6);

      expect(result.is_ok());
      expect(eq(result.unwrap(), std::vector<int>({ 1, 2, 3, -4, -5, 6 })));
    };

    it("test7") = [] {
      const std::vector<std::vector<int>> clauses{
        { 1 }, { -2, 4, 5 }, { -2, 6 },  { -3, 5 }, { 2 },
        { 3 }, { 4, 5 },     { -4, -5 }, { 6 }
      };
      const auto result = solve(clauses, 6);

      expect(result.is_ok());
      expect(eq(result.unwrap(), std::vector<int>({ 1, 2, 3, -4, 5, 6 })));
    };

    it("test8") = [] {
      const std::vector<std::vector<int>> clauses{ { 1 },      { 2 },
                                                   { 3 },      { -2, 4, 5, 6 },
                                                   { -3, 5 },  { 4, 5 },
                                                   { -4, -5 }, { 6 } };
      const auto result = solve(clauses, 6);

      expect(result.is_ok());
      expect(eq(result.unwrap(), std::vector<int>({ 1, 2, 3, -4, 5, 6 })));
    };
  };

  describe("test solve => UNSAT") = [] {
    it("test1") = [] {
      const std::vector<std::vector<int>> clauses{
        { 1 }, { -2, 4 }, { -3, 5 }, { 4, 5 }, { -4, -5 }, { 2 }, { 3 }
      };
      const auto result = solve(clauses, 5);

      expect(result.is_err());
    };

    // FIXME: Maybe the SAT solver has a bug
    skip / it("test2") = [] {
      const std::vector<std::vector<int>> clauses{
        { -1, -2, -3 }, { -2, -3, -4 }, { -2, -2, 3 }, { 2, 2, 2 }, { 1, -2, 4 }
      };
      const auto result = solve(clauses, 4);

      expect(result.is_err());
    };

    // FIXME: Maybe the SAT solver has a bug
    // c FILE: aim-100-1_6-no-1.cnf
    // c
    // c SOURCE: Kazuo Iwama, Eiji Miyano (miyano@cscu.kyushu-u.ac.jp),
    // c          and Yuichi Asahiro
    // c
    // c DESCRIPTION: Artifical instances from generator by source.  Generators
    // c              and more information in sat/contributed/iwama.
    // c
    // c NOTE: Not Satisfiable
    // c
    skip / it("test3") = [] {
      const std::vector<std::vector<int>> clauses{
        { 16, 30, 95 },    { -16, 30, 95 },    { -30, 35, 78 },
        { -30, -78, 85 },  { -78, -85, 95 },   { 8, 55, 100 },
        { 8, 55, -95 },    { 9, 52, 100 },     { 9, 73, -100 },
        { -8, -9, 52 },    { 38, 66, 83 },     { -38, 83, 87 },
        { -52, 83, -87 },  { 66, 74, -83 },    { -52, -66, 89 },
        { -52, 73, -89 },  { -52, 73, -74 },   { -8, -73, -95 },
        { 40, -55, 90 },   { -40, -55, 90 },   { 25, 35, 82 },
        { -25, 82, -90 },  { -55, -82, -90 },  { 11, 75, 84 },
        { 11, -75, 96 },   { 23, -75, -96 },   { -11, 23, -35 },
        { -23, 29, 65 },   { 29, -35, -65 },   { -23, -29, 84 },
        { -35, 54, 70 },   { -54, 70, 77 },    { 19, -77, -84 },
        { -19, -54, 70 },  { 22, 68, 81 },     { -22, 48, 81 },
        { -22, -48, 93 },  { 3, -48, -93 },    { 7, 18, -81 },
        { -7, 56, -81 },   { 3, 18, -56 },     { -18, 47, 68 },
        { -18, -47, -81 }, { -3, 68, 77 },     { -3, -77, -84 },
        { 19, -68, -70 },  { -19, -68, 74 },   { -68, -70, -74 },
        { 54, 61, -62 },   { 50, 53, -62 },    { -50, 61, -62 },
        { -27, 56, 93 },   { 4, 14, 76 },      { 4, -76, 96 },
        { -4, 14, 80 },    { -14, -68, 80 },   { -10, -39, -89 },
        { 1, 49, -81 },    { 1, 26, -49 },     { 17, -26, -49 },
        { -1, 17, -40 },   { 16, 51, -89 },    { -9, 57, 60 },
        { 12, 45, -51 },   { 2, 12, 69 },      { 2, -12, 40 },
        { -12, -51, 69 },  { -33, 60, -98 },   { 5, -32, -66 },
        { 2, -47, -100 },  { -42, 64, 83 },    { 20, -42, -64 },
        { 20, -48, 98 },   { -20, 50, 98 },    { -32, -50, 98 },
        { -24, 37, -73 },  { -24, -37, -100 }, { -57, 71, 81 },
        { -37, 40, -91 },  { 31, 42, 81 },     { -31, 42, 72 },
        { -31, 42, -72 },  { 7, -19, 25 },     { -1, -25, -94 },
        { -15, -44, 79 },  { -6, 31, 46 },     { -39, 41, 88 },
        { 28, -39, 43 },   { 28, -43, -88 },   { -4, -28, -88 },
        { -30, -39, -41 }, { -29, 33, 88 },    { -16, 21, 94 },
        { -10, 26, 62 },   { -11, -64, 86 },   { -6, -41, 76 },
        { 38, -46, 93 },   { 26, -37, 94 },    { -26, 53, -79 },
        { 78, 87, -94 },   { 65, 76, -87 },    { 23, 51, -62 },
        { -11, -36, 57 },  { 41, 59, -65 },    { -56, 72, -91 },
        { 13, -20, -46 },  { -13, 15, 79 },    { -17, 47, -60 },
        { -13, -44, 99 },  { -7, -38, 67 },    { 37, -49, 62 },
        { -14, -17, -79 }, { -13, -15, -22 },  { 32, -33, -34 },
        { 24, 45, 48 },    { 21, 24, -48 },    { -36, 64, -85 },
        { 10, -61, 67 },   { -5, 44, 59 },     { -80, -85, -99 },
        { 6, 37, -97 },    { -21, -34, 64 },   { -5, 44, 46 },
        { 58, -76, 97 },   { -21, -36, 75 },   { -15, 58, -59 },
        { -58, -76, -99 }, { -2, 15, 33 },     { -26, 34, -57 },
        { -18, -82, -92 }, { 27, -80, -97 },   { 6, 32, 63 },
        { -34, -86, 92 },  { 13, -61, 97 },    { -28, 43, -98 },
        { 5, 39, -86 },    { 39, -45, 92 },    { 27, -43, 97 },
        { 13, -58, -86 },  { -28, -67, -93 },  { -69, 85, 99 },
        { 42, 71, -72 },   { 10, -27, -63 },   { -59, 63, -83 },
        { 36, 86, -96 },   { -2, 36, 75 },     { -59, -71, 89 },
        { 36, -67, 91 },   { 36, -60, 63 },    { -63, 91, -93 },
        { 25, 87, 92 },    { -21, 49, -71 },   { -2, 10, 22 },
        { 6, -18, 41 },    { 6, 71, -92 },     { -53, -69, -71 },
        { -2, -53, -58 },  { 43, -45, -96 },   { 34, -45, -69 },
        { 63, -86, -98 }
      };
      const auto result = solve(clauses, 100);

      expect(result.is_err());
    };
  };
}
