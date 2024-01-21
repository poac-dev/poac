#include "Algos.hpp"

#include "Exception.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"

#include <algorithm>
#include <cctype>
#include <memory>
#include <utility>

String
toUpper(const StringRef str) noexcept {
  String res;
  for (const unsigned char c : str) {
    res += static_cast<char>(std::toupper(c));
  }
  return res;
}

String
toMacroName(StringRef name) noexcept {
  String macroName;
  for (const unsigned char c : name) {
    if (std::isalpha(c)) {
      macroName += static_cast<char>(std::toupper(c));
    } else if (std::isdigit(c)) {
      macroName += static_cast<char>(c);
    } else {
      macroName += '_';
    }
  }
  return macroName;
}

int
runCmd(const StringRef cmd) noexcept {
  Logger::debug("Running `", cmd, '`');
  const int status = std::system(cmd.data());
  const int exitCode = status >> 8;
  return exitCode;
}

String
getCmdOutput(const StringRef cmd) {
  std::array<char, 128> buffer;
  String result;

  Logger::debug("Running `", cmd, '`');
  const std::unique_ptr<FILE, decltype(&pclose)> pipe(
      popen(cmd.data(), "r"), pclose
  );
  if (!pipe) {
    throw PoacError("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

// ref: https://wandbox.org/permlink/zRjT41alOHdwcf00
static usize
levDistance(const StringRef a, const StringRef b) {
  const usize asize = a.size();
  const usize bsize = b.size();

  // for all i and j, d[i,j] will hold the Levenshtein distance between the
  // first i characters of s and the first j characters of t
  Vec<Vec<usize>> d(asize + 1, Vec<usize>(bsize + 1));
  d[0][0] = 0;

  // source prefixes can be transformed into empty string by dropping all
  // characters
  for (usize i = 1; i <= asize; ++i) {
    d[i][0] = i;
  }

  // target prefixes can be reached from empty source prefix by inserting every
  // character
  for (usize j = 1; j <= bsize; ++j) {
    d[0][j] = j;
  }

  for (usize i = 1; i <= asize; ++i) {
    for (usize j = 1; j <= bsize; ++j) {
      const usize substCost = a[i - 1] == b[j - 1] ? 0 : 1;
      d[i][j] = std::min({
          d[i - 1][j] + 1, // deletion
          d[i][j - 1] + 1, // insertion
          d[i - 1][j - 1] + substCost // substitution
      });
    }
  }

  return d[asize][bsize];
}

static bool
equalsInsensitive(const StringRef a, const StringRef b) {
  return std::equal(
      a.cbegin(), a.cend(), b.cbegin(), b.cend(),
      [](char a, char b) { return std::tolower(a) == std::tolower(b); }
  );
}

Option<StringRef>
findSimilarStr(const StringRef lhs, std::span<const StringRef> candidates) {
  // We need to check if `Candidates` has the exact case-insensitive string
  // because the Levenshtein distance match does not care about it.
  for (const StringRef c : candidates) {
    if (equalsInsensitive(lhs, c)) {
      return c;
    }
  }

  // Keep going with the Levenshtein distance match.
  // If the LHS size is less than 3, use the LHS size minus 1 and if not,
  // use the LHS size divided by 3.
  const usize length = lhs.size();
  const usize maxDist = length < 3 ? length - 1 : length / 3;

  Option<std::pair<StringRef, usize>> similarStr = None;
  for (const StringRef c : candidates) {
    const usize curDist = levDistance(lhs, c);
    if (curDist <= maxDist) {
      // The first similar string found || More similar string found
      if (!similarStr.has_value() || curDist < similarStr->second) {
        similarStr = { c, curDist };
      }
    }
  }

  if (similarStr.has_value()) {
    return similarStr->first;
  } else {
    return None;
  }
}

#ifdef POAC_TEST

#  include <limits>

namespace tests {

void
testLevDistance() {
  // Test bytelength agnosticity
  for (char c = 0; c < std::numeric_limits<char>::max(); ++c) {
    const String str(1, c);
    assertEq(levDistance(str, str), 0UL);
  }

  pass();
}

void
testLevDistance2() {
  constexpr StringRef A = "\nMäry häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef B = "\nMary häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef C = "Mary häd ä little lämb\n\nLittle lämb\n";

  assertEq(levDistance(A, B), 2UL);
  assertEq(levDistance(B, A), 2UL);
  assertEq(levDistance(A, C), 3UL);
  assertEq(levDistance(C, A), 3UL);
  assertEq(levDistance(B, C), 1UL);
  assertEq(levDistance(C, B), 1UL);

  assertEq(levDistance("b", "bc"), 1UL);
  assertEq(levDistance("ab", "abc"), 1UL);
  assertEq(levDistance("aab", "aabc"), 1UL);
  assertEq(levDistance("aaab", "aaabc"), 1UL);

  assertEq(levDistance("a", "b"), 1UL);
  assertEq(levDistance("ab", "ac"), 1UL);
  assertEq(levDistance("aab", "aac"), 1UL);
  assertEq(levDistance("aaab", "aaac"), 1UL);

  pass();
}

// ref:
// https://github.com/llvm/llvm-project/commit/a247ba9d15635d96225ef39c8c150c08f492e70a#diff-fd993637669817b267190e7de029b75af5a0328d43d9b70c2e8dd512512091a2

void
testFindSimilarStr() {
  constexpr Arr<StringRef, 8> CANDIDATES{
    "if", "ifdef", "ifndef", "elif", "else", "endif", "elifdef", "elifndef"
  };

  assertEq(findSimilarStr("id", CANDIDATES), "if"sv);
  assertEq(findSimilarStr("ifd", CANDIDATES), "if"sv);
  assertEq(findSimilarStr("ifde", CANDIDATES), "ifdef"sv);
  assertEq(findSimilarStr("elf", CANDIDATES), "elif"sv);
  assertEq(findSimilarStr("elsif", CANDIDATES), "elif"sv);
  assertEq(findSimilarStr("elseif", CANDIDATES), "elif"sv);
  assertEq(findSimilarStr("elfidef", CANDIDATES), "elifdef"sv);
  assertEq(findSimilarStr("elfindef", CANDIDATES), "elifdef"sv);
  assertEq(findSimilarStr("elfinndef", CANDIDATES), "elifndef"sv);
  assertEq(findSimilarStr("els", CANDIDATES), "else"sv);
  assertEq(findSimilarStr("endi", CANDIDATES), "endif"sv);

  assertEq(findSimilarStr("i", CANDIDATES), None);
  assertEq(findSimilarStr("special_compiler_directive", CANDIDATES), None);

  pass();
}

void
testFindSimilarStr2() {
  constexpr Arr<StringRef, 2> CANDIDATES{ "aaab", "aaabc" };
  assertEq(findSimilarStr("aaaa", CANDIDATES), "aaab"sv);
  assertEq(findSimilarStr("1111111111", CANDIDATES), None);

  constexpr Arr<StringRef, 1> CANDIDATES2{ "AAAA" };
  assertEq(findSimilarStr("aaaa", CANDIDATES2), "AAAA"sv);

  pass();
}

} // namespace tests

int
main() {
  tests::testLevDistance();
  tests::testLevDistance2();
  tests::testFindSimilarStr();
  tests::testFindSimilarStr2();
}

#endif
