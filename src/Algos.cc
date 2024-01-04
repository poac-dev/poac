#include "Algos.hpp"

#include "Exception.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"

#include <algorithm>
#include <memory>
#include <utility>

int runCmd(StringRef cmd) {
  Logger::debug("Running `", cmd, '`');
  const int status = std::system(cmd.data());
  const int exitCode = status >> 8;
  return exitCode;
}

String getCmdOutput(StringRef cmd) {
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

// O(M) where M is the length of the word.
void trieInsert(TrieNode& root, StringRef word) {
  TrieNode* node = &root;
  for (const char ch : word) {
    if (!node->children.contains(ch)) {
      node->children[ch] = std::make_unique<TrieNode>();
    }
    node = node->children[ch].get();
  }
  node->isEndOfWord = true;
}

// O(M) where M is the length of the word.
bool trieSearch(const TrieNode& root, StringRef word) {
  const TrieNode* node = &root;
  for (const char ch : word) {
    if (!node->children.contains(ch)) {
      return false;
    }
    node = node->children.at(ch).get();
    if (node->isEndOfWord) {
      return true;
    }
  }
  return false;
}

// O(M^2) where M is the length of the word.
bool trieSearchFromAnyPosition(const TrieNode& root, StringRef word) {
  for (usize i = 0; i < word.size(); ++i) {
    const TrieNode* node = &root;
    for (usize j = i; j < word.size(); ++j) {
      const char ch = word[j];
      if (!node->children.contains(ch)) {
        break;
      }
      node = node->children.at(ch).get();
      if (node->isEndOfWord) {
        return true;
      }
    }
  }
  return false;
}

// ref: https://wandbox.org/permlink/zRjT41alOHdwcf00
static usize levDistance(StringRef a, StringRef b) {
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

static bool equalsInsensitive(StringRef a, StringRef b) {
  return std::equal(
      a.cbegin(), a.cend(), b.cbegin(), b.cend(),
      [](char a, char b) { return std::tolower(a) == std::tolower(b); }
  );
}

Option<StringRef>
findSimilarStr(StringRef lhs, std::span<const StringRef> candidates) {
  // We need to check if `Candidates` has the exact case-insensitive string
  // because the Levenshtein distance match does not care about it.
  for (StringRef c : candidates) {
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

#  include "TestUtils.hpp"

#  include <limits>

void testLevDistance() {
  // Test bytelength agnosticity
  for (char c = 0; c < std::numeric_limits<char>::max(); ++c) {
    const String str(1, c);
    ASSERT_EQ(levDistance(str, str), 0);
  }
}

void testLevDistance2() {
  constexpr StringRef A = "\nMäry häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef B = "\nMary häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef C = "Mary häd ä little lämb\n\nLittle lämb\n";

  ASSERT_EQ(levDistance(A, B), 2);
  ASSERT_EQ(levDistance(B, A), 2);
  ASSERT_EQ(levDistance(A, C), 3);
  ASSERT_EQ(levDistance(C, A), 3);
  ASSERT_EQ(levDistance(B, C), 1);
  ASSERT_EQ(levDistance(C, B), 1);

  ASSERT_EQ(levDistance("b", "bc"), 1);
  ASSERT_EQ(levDistance("ab", "abc"), 1);
  ASSERT_EQ(levDistance("aab", "aabc"), 1);
  ASSERT_EQ(levDistance("aaab", "aaabc"), 1);

  ASSERT_EQ(levDistance("a", "b"), 1);
  ASSERT_EQ(levDistance("ab", "ac"), 1);
  ASSERT_EQ(levDistance("aab", "aac"), 1);
  ASSERT_EQ(levDistance("aaab", "aaac"), 1);
}

// ref:
// https://github.com/llvm/llvm-project/commit/a247ba9d15635d96225ef39c8c150c08f492e70a#diff-fd993637669817b267190e7de029b75af5a0328d43d9b70c2e8dd512512091a2

void testFindSimilarStr() {
  constexpr Arr<StringRef, 8> CANDIDATES{
    "if", "ifdef", "ifndef", "elif", "else", "endif", "elifdef", "elifndef"
  };

  ASSERT_EQ(findSimilarStr("id", CANDIDATES), "if"sv);
  ASSERT_EQ(findSimilarStr("ifd", CANDIDATES), "if"sv);
  ASSERT_EQ(findSimilarStr("ifde", CANDIDATES), "ifdef"sv);
  ASSERT_EQ(findSimilarStr("elf", CANDIDATES), "elif"sv);
  ASSERT_EQ(findSimilarStr("elsif", CANDIDATES), "elif"sv);
  ASSERT_EQ(findSimilarStr("elseif", CANDIDATES), "elif"sv);
  ASSERT_EQ(findSimilarStr("elfidef", CANDIDATES), "elifdef"sv);
  ASSERT_EQ(findSimilarStr("elfindef", CANDIDATES), "elifdef"sv);
  ASSERT_EQ(findSimilarStr("elfinndef", CANDIDATES), "elifndef"sv);
  ASSERT_EQ(findSimilarStr("els", CANDIDATES), "else"sv);
  ASSERT_EQ(findSimilarStr("endi", CANDIDATES), "endif"sv);

  ASSERT_EQ(findSimilarStr("i", CANDIDATES), None);
  ASSERT_EQ(findSimilarStr("special_compiler_directive", CANDIDATES), None);
}

void testFindSimilarStr2() {
  constexpr Arr<StringRef, 2> CANDIDATES{ "aaab", "aaabc" };
  ASSERT_EQ(findSimilarStr("aaaa", CANDIDATES), "aaab"sv);
  ASSERT_EQ(findSimilarStr("1111111111", CANDIDATES), None);

  constexpr Arr<StringRef, 1> CANDIDATES2{ "AAAA" };
  ASSERT_EQ(findSimilarStr("aaaa", CANDIDATES2), "AAAA"sv);
}

int main() {
  REGISTER_TEST(testLevDistance);
  REGISTER_TEST(testLevDistance2);
  REGISTER_TEST(testFindSimilarStr);
  REGISTER_TEST(testFindSimilarStr2);
}

#endif
