#pragma once

#include "Rustify.hpp"

#include <iostream>
#include <queue>
#include <span>
#include <stdexcept>

template <typename T>
Vec<String> topoSort(
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
    throw std::runtime_error("too complex build graph");
  }
  return res;
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
Option<StringRef>
findSimilarStr(StringRef lhs, std::span<const StringRef> candidates);
