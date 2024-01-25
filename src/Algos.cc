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
execCmd(const StringRef cmd) noexcept {
  Logger::debug("Running `", cmd, '`');
  const int status = std::system(cmd.data());
  const int exitCode = status >> 8;
  return exitCode;
}

String
getCmdOutput(const StringRef cmd) {
  std::array<char, 128> buffer{};
  String result;

  Logger::debug("Running `", cmd, '`');
  FILE* pipe = popen(cmd.data(), "r");
  if (!pipe) {
    throw PoacError("popen() failed!");
  }

  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    result += buffer.data();
  }

  const int status = pclose(pipe);
  if (status == -1) {
    throw PoacError("pclose() failed!");
  }
  const int exitCode = status >> 8;
  if (exitCode != EXIT_SUCCESS) {
    std::cerr << result;
    throw PoacError("Command failed with exit code ", exitCode);
  }
  return result;
}

bool
commandExists(const StringRef cmd) noexcept {
  String checkCmd = "command -v ";
  checkCmd += cmd;
  checkCmd += " >/dev/null 2>&1";
  return execCmd(checkCmd) == EXIT_SUCCESS;
}

// ref: https://wandbox.org/permlink/zRjT41alOHdwcf00
static usize
levDistance(const StringRef lhs, const StringRef rhs) {
  const usize lhsSize = lhs.size();
  const usize rhsSize = rhs.size();

  // for all i and j, d[i,j] will hold the Levenshtein distance between the
  // first i characters of s and the first j characters of t
  Vec<Vec<usize>> dist(lhsSize + 1, Vec<usize>(rhsSize + 1));
  dist[0][0] = 0;

  // source prefixes can be transformed into empty string by dropping all
  // characters
  for (usize i = 1; i <= lhsSize; ++i) {
    dist[i][0] = i;
  }

  // target prefixes can be reached from empty source prefix by inserting every
  // character
  for (usize j = 1; j <= rhsSize; ++j) {
    dist[0][j] = j;
  }

  for (usize i = 1; i <= lhsSize; ++i) {
    for (usize j = 1; j <= rhsSize; ++j) {
      const usize substCost = lhs[i - 1] == rhs[j - 1] ? 0 : 1;
      dist[i][j] = std::min({
          dist[i - 1][j] + 1, // deletion
          dist[i][j - 1] + 1, // insertion
          dist[i - 1][j - 1] + substCost // substitution
      });
    }
  }

  return dist[lhsSize][rhsSize];
}

static bool
equalsInsensitive(const StringRef lhs, const StringRef rhs) {
  return std::equal(
      lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(),
      [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }
  );
}

Option<StringRef>
findSimilarStr(const StringRef lhs, std::span<const StringRef> candidates) {
  // We need to check if `Candidates` has the exact case-insensitive string
  // because the Levenshtein distance match does not care about it.
  for (const StringRef str : candidates) {
    if (equalsInsensitive(lhs, str)) {
      return str;
    }
  }

  // Keep going with the Levenshtein distance match.
  // If the LHS size is less than 3, use the LHS size minus 1 and if not,
  // use the LHS size divided by 3.
  const usize length = lhs.size();
  const usize maxDist = length < 3 ? length - 1 : length / 3;

  Option<std::pair<StringRef, usize>> similarStr = None;
  for (const StringRef str : candidates) {
    const usize curDist = levDistance(lhs, str);
    if (curDist <= maxDist) {
      // The first similar string found || More similar string found
      if (!similarStr.has_value() || curDist < similarStr->second) {
        similarStr = { str, curDist };
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
  constexpr StringRef STR1 = "\nMäry häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef STR2 = "\nMary häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef STR3 = "Mary häd ä little lämb\n\nLittle lämb\n";

  assertEq(levDistance(STR1, STR2), 2UL);
  assertEq(levDistance(STR2, STR1), 2UL);
  assertEq(levDistance(STR1, STR3), 3UL);
  assertEq(levDistance(STR3, STR1), 3UL);
  assertEq(levDistance(STR2, STR3), 1UL);
  assertEq(levDistance(STR3, STR2), 1UL);

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
