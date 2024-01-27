#pragma once

#include "Exception.hpp"
#include "Rustify.hpp"

#include <initializer_list>
#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <span>
#include <sstream>
#include <utility>

String toUpper(StringRef str) noexcept;
String toMacroName(StringRef name) noexcept;

int execCmd(StringRef cmd) noexcept;
String getCmdOutput(StringRef cmd);
bool commandExists(StringRef cmd) noexcept;

template <typename T>
Vec<String>
topoSort(
    const HashMap<String, T>& list, const HashMap<String, Vec<String>>& adjList
) {
  HashMap<String, u32> inDegree;
  for (const auto& var : list) {
    inDegree[var.first] = 0;
  }
  for (const auto& edge : adjList) {
    if (!list.contains(edge.first)) {
      continue; // Ignore nodes not in list
    }
    if (!inDegree.contains(edge.first)) {
      inDegree[edge.first] = 0;
    }
    for (const auto& neighbor : edge.second) {
      inDegree[neighbor]++;
    }
  }

  std::queue<String> zeroInDegree;
  for (const auto& var : inDegree) {
    if (var.second == 0) {
      zeroInDegree.push(var.first);
    }
  }

  Vec<String> res;
  while (!zeroInDegree.empty()) {
    const String node = zeroInDegree.front();
    zeroInDegree.pop();
    res.push_back(node);

    if (!adjList.contains(node)) {
      // No dependencies
      continue;
    }
    for (const String& neighbor : adjList.at(node)) {
      inDegree[neighbor]--;
      if (inDegree[neighbor] == 0) {
        zeroInDegree.push(neighbor);
      }
    }
  }

  if (res.size() != list.size()) {
    // Cycle detected
    throw PoacError("too complex build graph");
  }
  return res;
}

// ref: https://wandbox.org/permlink/zRjT41alOHdwcf00
constexpr usize
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

constexpr char
toLower(const char c) {
  return (c >= 'A' && c <= 'Z') ? c + static_cast<char>(('a' - 'A')) : c;
}

constexpr bool
equalsInsensitive(const StringRef lhs, const StringRef rhs) {
  return std::equal(
      lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(),
      [](char lhs, char rhs) { return toLower(lhs) == toLower(rhs); }
  );
}

// ref: https://reviews.llvm.org/differential/changeset/?ref=3315514
/// Find a similar string in `candidates`.
///
/// \param lhs a string for a similar string in `Candidates`
///
/// \param candidates the candidates to find a similar string.
///
/// \returns a similar string if exists. If no similar string exists,
/// returns None.
constexpr Option<StringRef>
findSimilarStr(
    const StringRef lhs, const std::span<const StringRef> candidates
) {
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
