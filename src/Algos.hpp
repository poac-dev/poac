#pragma once

#include "Rustify.hpp"

#include <initializer_list>
#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <span>
#include <sstream>
#include <stdexcept>
#include <utility>

int runCmd(StringRef);
String getCmdOutput(StringRef);

template <typename... Args>
inline String concat(Args&&... args) {
  return (std::ostringstream{} << ... << std::forward<Args>(args)).str();
}

template <typename Value>
struct OrderedHashSet {
  using Iterator = typename Vec<Value>::iterator;
  using ConstIterator = typename Vec<Value>::const_iterator;

  OrderedHashSet() = default;
  OrderedHashSet(std::initializer_list<Value> init) {
    for (const Value& value : init) {
      push_back(value);
    }
  }

  OrderedHashSet(const OrderedHashSet&) = default;
  OrderedHashSet& operator=(const OrderedHashSet&) = default;
  OrderedHashSet(OrderedHashSet&&) noexcept = default;
  OrderedHashSet& operator=(OrderedHashSet&&) noexcept = default;

  // O(1)
  void push_back(const Value& value) {
    if (!set.contains(value)) {
      list.push_back(value);
      set.insert(value);
    }
  }

  // O(1)
  const Value& operator[](usize index) const {
    return list[index];
  }
  // O(1)
  Value& operator[](const Value& value) {
    if (!set.contains(value)) {
      list.push_back(value);
    }
    return set[value];
  }

  // O(1)
  const Value& at(const Value& value) const {
    auto it = set.find(value);
    if (it == set.end()) {
      throw std::out_of_range("Value not found");
    }
    return *it;
  }

  // O(1)
  bool contains(const Value& value) const {
    return set.contains(value);
  }

  Iterator begin() {
    return list.begin();
  }
  ConstIterator begin() const {
    return list.begin();
  }

  Iterator end() {
    return list.end();
  }
  ConstIterator end() const {
    return list.end();
  }

  operator std::span<Value>() {
    return std::span<Value>(&*list.begin(), list.size());
  }
  operator std::span<const Value>() const {
    return std::span<const Value>(&*list.begin(), list.size());
  }

private:
  Vec<Value> list;
  HashSet<Value> set;
};

struct TrieNode {
  HashMap<char, std::unique_ptr<TrieNode>> children;
  bool isEndOfWord = false;
};
void trieInsert(TrieNode&, StringRef);
bool trieSearch(const TrieNode&, StringRef);
bool trieSearchFromAnyPosition(const TrieNode&, StringRef);

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
Option<StringRef> findSimilarStr(StringRef, std::span<const StringRef>);
