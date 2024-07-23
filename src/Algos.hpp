#pragma once

#include "Exception.hpp"
#include "Rustify.hpp"

#include <initializer_list>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <queue>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

std::string toUpper(std::string_view str) noexcept;
std::string toMacroName(std::string_view name) noexcept;

int execCmd(std::string_view cmd) noexcept;
std::string getCmdOutput(std::string_view cmd, usize retry = 3);
bool commandExists(std::string_view cmd) noexcept;

template <typename T>
std::vector<std::string>
topoSort(
    const std::unordered_map<std::string, T>& list,
    const std::unordered_map<std::string, std::vector<std::string>>& adjList
) {
  std::unordered_map<std::string, u32> inDegree;
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

  std::queue<std::string> zeroInDegree;
  for (const auto& var : inDegree) {
    if (var.second == 0) {
      zeroInDegree.push(var.first);
    }
  }

  std::vector<std::string> res;
  while (!zeroInDegree.empty()) {
    const std::string node = zeroInDegree.front();
    zeroInDegree.pop();
    res.push_back(node);

    if (!adjList.contains(node)) {
      // No dependencies
      continue;
    }
    for (const std::string& neighbor : adjList.at(node)) {
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

// ref: https://reviews.llvm.org/differential/changeset/?ref=3315514
/// Find a similar string in `candidates`.
///
/// \param lhs a string for a similar string in `Candidates`
///
/// \param candidates the candidates to find a similar string.
///
/// \returns a similar string if exists. If no similar string exists,
/// returns std::nullopt.
std::optional<std::string_view> findSimilarStr(
    std::string_view lhs, std::span<const std::string_view> candidates
);
