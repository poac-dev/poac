#include "Algos.hpp"

#include "Rustify.hpp"

#include <algorithm>
#include <memory>
#include <utility>

// O(M) where M is the length of the word.
void trieInsert(TrieNode& root, StringRef word) {
  TrieNode* node = &root;
  for (char ch : word) {
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
  for (char ch : word) {
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
  for (size_t i = 0; i < word.size(); ++i) {
    const TrieNode* node = &root;
    for (size_t j = i; j < word.size(); ++j) {
      char ch = word[j];
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
      const usize subst_cost = a[i - 1] == b[j - 1] ? 0 : 1;
      d[i][j] = std::min({
          d[i - 1][j] + 1, // deletion
          d[i][j - 1] + 1, // insertion
          d[i - 1][j - 1] + subst_cost // substitution
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
  const usize max_dist = length < 3 ? length - 1 : length / 3;

  Option<std::pair<StringRef, usize>> similar_str = None;
  for (const StringRef c : candidates) {
    const usize cur_dist = levDistance(lhs, c);
    if (cur_dist <= max_dist) {
      // The first similar string found || More similar string found
      if (!similar_str.has_value() || cur_dist < similar_str->second) {
        similar_str = {c, cur_dist};
      }
    }
  }

  if (similar_str.has_value()) {
    return similar_str->first;
  } else {
    return None;
  }
}

#ifdef POAC_TEST

#  include <cassert>
#  include <limits>

void test_levDistance() {
  // Test bytelength agnosticity
  for (char c = 0; c < std::numeric_limits<char>::max(); ++c) {
    String str = String(1, c);
    assert(levDistance(str, str) == 0);
  }
}

void test_levDistance2() {
  constexpr StringRef A = "\nMäry häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef B = "\nMary häd ä little lämb\n\nLittle lämb\n";
  constexpr StringRef C = "Mary häd ä little lämb\n\nLittle lämb\n";

  assert(levDistance(A, B) == 2);
  assert(levDistance(B, A) == 2);
  assert(levDistance(A, C) == 3);
  assert(levDistance(C, A) == 3);
  assert(levDistance(B, C) == 1);
  assert(levDistance(C, B) == 1);

  assert(levDistance("b", "bc") == 1);
  assert(levDistance("ab", "abc") == 1);
  assert(levDistance("aab", "aabc") == 1);
  assert(levDistance("aaab", "aaabc") == 1);

  assert(levDistance("a", "b") == 1);
  assert(levDistance("ab", "ac") == 1);
  assert(levDistance("aab", "aac") == 1);
  assert(levDistance("aaab", "aaac") == 1);
}

// ref:
// https://github.com/llvm/llvm-project/commit/a247ba9d15635d96225ef39c8c150c08f492e70a#diff-fd993637669817b267190e7de029b75af5a0328d43d9b70c2e8dd512512091a2

void test_findSimilarStr() {
  constexpr Arr<StringRef, 8> CANDIDATES{"if",      "ifdef",   "ifndef",
                                         "elif",    "else",    "endif",
                                         "elifdef", "elifndef"};

  assert(findSimilarStr("id", CANDIDATES) == "if"sv);
  assert(findSimilarStr("ifd", CANDIDATES) == "if"sv);
  assert(findSimilarStr("ifde", CANDIDATES) == "ifdef"sv);
  assert(findSimilarStr("elf", CANDIDATES) == "elif"sv);
  assert(findSimilarStr("elsif", CANDIDATES) == "elif"sv);
  assert(findSimilarStr("elseif", CANDIDATES) == "elif"sv);
  assert(findSimilarStr("elfidef", CANDIDATES) == "elifdef"sv);
  assert(findSimilarStr("elfindef", CANDIDATES) == "elifdef"sv);
  assert(findSimilarStr("elfinndef", CANDIDATES) == "elifndef"sv);
  assert(findSimilarStr("els", CANDIDATES) == "else"sv);
  assert(findSimilarStr("endi", CANDIDATES) == "endif"sv);

  assert(findSimilarStr("i", CANDIDATES) == None);
  assert(findSimilarStr("special_compiler_directive", CANDIDATES) == None);
}

void test_findSimilarStr2() {
  constexpr Arr<StringRef, 2> CANDIDATES{"aaab", "aaabc"};
  assert(findSimilarStr("aaaa", CANDIDATES) == "aaab"sv);
  assert(findSimilarStr("1111111111", CANDIDATES) == None);

  constexpr Arr<StringRef, 1> CANDIDATES2{"AAAA"};
  assert(findSimilarStr("aaaa", CANDIDATES2) == "AAAA"sv);
}

int main() {
  test_levDistance();
  test_levDistance2();
  test_findSimilarStr();
  test_findSimilarStr2();
}

#endif
