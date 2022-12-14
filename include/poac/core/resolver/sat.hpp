#pragma once

// std
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory> // std::allocator
#include <numeric>
#include <utility>

// external
#include <boost/range/adaptor/indexed.hpp>

// internal
#include "poac/poac.hpp"

namespace poac::core::resolver::sat {

enum class Status {
  satisfied, // found a satisfying assignment
  unsatisfied, // found no satisfying assignment
  normal, // Successful completion OR unsolved
};

Fn to_assignments(const Vec<i32>& literals)->Vec<i32>;

// the difference in number of occurrences
template <
    template <class T, class = std::allocator<T>> typename TwoDim,
    template <class T, class = std::allocator<T>> typename OneDim, typename T,
    typename U>
Fn calc_literal_polarity(const TwoDim<OneDim<T>>& rng, const U& i)->T {
  T acc = 0;
  for (const auto& rn : rng) {
    for (const auto& r : rn) {
      if (std::abs(r) == i) {
        r > 0 ? ++acc : --acc;
      }
    }
  }
  return acc;
}

inline Fn literal_to_index(i32 l)->i32 { return std::abs(l) - 1; }

// Find `1` or `-1` from whole clauses. Variables that have already been
// assigned have been deleted from the clauses by the `delete_applied_literal`
// function, so the index of the variable with the highest number of variables
// is returned from the variables in the current clauses.
Fn maximum_literal_number_index(const Vec<Vec<i32>>& clauses)->i32;

// Delete variables from the clauses for which variable assignment has been
// determined.
Fn delete_set_literal(
    Vec<Vec<i32>>& clauses, const i32& index, const i32& set_val
)
    ->Status;

// unit resolution
Fn unit_propagate(Vec<Vec<i32>>& clauses, Vec<i32>& literals)->Status;

// recursive DPLL algorithm
[[nodiscard]] Fn dpll(Vec<Vec<i32>>& clauses, Vec<i32>& literals)
    ->Result<Vec<i32>, String>;

[[nodiscard]] inline Fn solve(Vec<Vec<i32>> clauses, const u32& variables)
    ->Result<Vec<i32>, String> {
  // Express the assignment status of the literal value corresponding to index.
  // a vector that stores the value assigned to each variable, where
  // -1 - unassigned, 0 - true, 1 - false.
  Vec<i32> literals(variables, -1);
  return dpll(clauses, literals);
}

} // namespace poac::core::resolver::sat
