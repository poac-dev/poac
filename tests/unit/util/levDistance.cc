// std
#include <array>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

// external
#include <boost/ut.hpp>

// internal
import poac.util.levDistance;

// Some tests came from:
// https://github.com/ken-matsui/lev_distance/blob/main/src/lib.rs

auto main() -> int {
  using namespace poac;
  using namespace boost::ut;

  // TODO: not exported function
  //  "test lev_distance"_test = [] {
  //    using util::lev_distance::calc;
  //
  //    // Test bytelength agnosticity
  //    for (char c = 0; c < std::numeric_limits<char>::max(); ++c) {
  //      expect(eq(calc(std::string(1, c), std::string(1, c)), 0));
  //    }
  //
  //    constexpr StringRef A = "\nMäry häd ä little lämb\n\nLittle lämb\n";
  //    constexpr StringRef B = "\nMary häd ä little lämb\n\nLittle lämb\n";
  //    constexpr StringRef C = "Mary häd ä little lämb\n\nLittle lämb\n";
  //    expect(eq(calc(A, B), 2));
  //    expect(eq(calc(B, A), 2));
  //    expect(eq(calc(A, C), 3));
  //    expect(eq(calc(C, A), 3));
  //    expect(eq(calc(B, C), 1));
  //    expect(eq(calc(C, B), 1));
  //
  //    expect(eq(calc("b", "bc"), 1));
  //    expect(eq(calc("ab", "abc"), 1));
  //    expect(eq(calc("aab", "aabc"), 1));
  //    expect(eq(calc("aaab", "aaabc"), 1));
  //
  //    expect(eq(calc("a", "b"), 1));
  //    expect(eq(calc("ab", "ac"), 1));
  //    expect(eq(calc("aab", "aac"), 1));
  //    expect(eq(calc("aaab", "aaac"), 1));
  //  };

  // ref:
  // https://github.com/llvm/llvm-project/commit/a247ba9d15635d96225ef39c8c150c08f492e70a#diff-fd993637669817b267190e7de029b75af5a0328d43d9b70c2e8dd512512091a2
  "test find_similar_str"_test = [] {
    using util::lev_distance::find_similar_str;
    using namespace std::literals::string_view_literals;

    constexpr std::array<std::string_view, 8> CANDIDATES{
        "if",   "ifdef", "ifndef",  "elif",
        "else", "endif", "elifdef", "elifndef"};
    expect(find_similar_str("id", CANDIDATES) == "if"sv);
    expect(find_similar_str("ifd", CANDIDATES) == "if"sv);
    expect(find_similar_str("ifde", CANDIDATES) == "ifdef"sv);
    expect(find_similar_str("elf", CANDIDATES) == "elif"sv);
    expect(find_similar_str("elsif", CANDIDATES) == "elif"sv);
    expect(find_similar_str("elseif", CANDIDATES) == "elif"sv);
    expect(find_similar_str("elfidef", CANDIDATES) == "elifdef"sv);
    expect(find_similar_str("elfindef", CANDIDATES) == "elifdef"sv);
    expect(find_similar_str("elfinndef", CANDIDATES) == "elifndef"sv);
    expect(find_similar_str("els", CANDIDATES) == "else"sv);
    expect(find_similar_str("endi", CANDIDATES) == "endif"sv);

    expect(find_similar_str("i", CANDIDATES) == std::nullopt);
    expect(
        find_similar_str("special_compiler_directive", CANDIDATES)
        == std::nullopt
    );
  };

  "test find_similar_str 2"_test = [] {
    using util::lev_distance::find_similar_str;
    using namespace std::literals::string_view_literals;

    constexpr std::array<std::string_view, 2> CANDIDATES{"aaab", "aaabc"};
    expect(find_similar_str("aaaa", CANDIDATES) == "aaab"sv);
    expect(find_similar_str("1111111111", CANDIDATES) == std::nullopt);

    constexpr std::array<std::string_view, 1> CANDIDATES2{"AAAA"};
    expect(find_similar_str("aaaa", CANDIDATES2) == "AAAA"sv);
  };
}
