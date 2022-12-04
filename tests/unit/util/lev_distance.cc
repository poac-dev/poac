// std
#include <limits>

// external
#include <boost/ut.hpp>

// internal
#include <poac/util/lev_distance.hpp>

// Some tests come from:
// https://github.com/ken-matsui/lev_distance/blob/main/src/lib.rs

int
main() {
  using namespace poac;
  using namespace boost::ut;

  "test lev_distance"_test = [] {
    using util::lev_distance::calc;

    // Test bytelength agnosticity
    for (char c = 0; c < std::numeric_limits<char>::max(); ++c) {
      expect(eq(calc(String(1, c), String(1, c)), 0));
    }

    constexpr StringRef a = "\nMäry häd ä little lämb\n\nLittle lämb\n";
    constexpr StringRef b = "\nMary häd ä little lämb\n\nLittle lämb\n";
    constexpr StringRef c = "Mary häd ä little lämb\n\nLittle lämb\n";
    expect(eq(calc(a, b), 2));
    expect(eq(calc(b, a), 2));
    expect(eq(calc(a, c), 3));
    expect(eq(calc(c, a), 3));
    expect(eq(calc(b, c), 1));
    expect(eq(calc(c, b), 1));

    expect(eq(calc("b", "bc"), 1));
    expect(eq(calc("ab", "abc"), 1));
    expect(eq(calc("aab", "aabc"), 1));
    expect(eq(calc("aaab", "aaabc"), 1));

    expect(eq(calc("a", "b"), 1));
    expect(eq(calc("ab", "ac"), 1));
    expect(eq(calc("aab", "aac"), 1));
    expect(eq(calc("aaab", "aaac"), 1));
  };

  // ref:
  // https://github.com/llvm/llvm-project/commit/a247ba9d15635d96225ef39c8c150c08f492e70a#diff-fd993637669817b267190e7de029b75af5a0328d43d9b70c2e8dd512512091a2
  "test find_similar_str"_test = [] {
    using util::lev_distance::find_similar_str;

    constexpr Arr<StringRef, 8> candidates{"if",      "ifdef",   "ifndef",
                                           "elif",    "else",    "endif",
                                           "elifdef", "elifndef"};
    expect(find_similar_str("id", candidates) == "if"sv);
    expect(find_similar_str("ifd", candidates) == "if"sv);
    expect(find_similar_str("ifde", candidates) == "ifdef"sv);
    expect(find_similar_str("elf", candidates) == "elif"sv);
    expect(find_similar_str("elsif", candidates) == "elif"sv);
    expect(find_similar_str("elseif", candidates) == "elif"sv);
    expect(find_similar_str("elfidef", candidates) == "elifdef"sv);
    expect(find_similar_str("elfindef", candidates) == "elifdef"sv);
    expect(find_similar_str("elfinndef", candidates) == "elifndef"sv);
    expect(find_similar_str("els", candidates) == "else"sv);
    expect(find_similar_str("endi", candidates) == "endif"sv);

    expect(find_similar_str("i", candidates) == None);
    expect(find_similar_str("special_compiler_directive", candidates) == None);
  };

  "test find_similar_str 2"_test = [] {
    using util::lev_distance::find_similar_str;

    constexpr Arr<StringRef, 2> candidates{"aaab", "aaabc"};
    expect(find_similar_str("aaaa", candidates) == "aaab"sv);
    expect(find_similar_str("1111111111", candidates) == None);

    constexpr Arr<StringRef, 1> candidates2{"AAAA"};
    expect(find_similar_str("aaaa", candidates2) == "AAAA"sv);
  };
}
