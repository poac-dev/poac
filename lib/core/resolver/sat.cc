// internal
#include "poac/core/resolver/sat.hpp"

namespace poac::core::resolver::sat {

Fn to_assignments(const Vec<i32>& literals)->Vec<i32> {
  Vec<i32> assignments;
  for (auto&& l : literals | boost::adaptors::indexed()) {
    const i32 literal = l.index() + 1;
    if (l.value() != -1) {
      assignments.emplace_back((l.value() % 2 == 0 ? 1 : -1) * literal);
    } else { // for literals that can take either value, arbitrarily assign them
             // to be true
      assignments.emplace_back(literal);
    }
  }
  return assignments;
}

// Find `1` or `-1` from whole clauses. Variables that have already been
// assigned have been deleted from the clauses by the `delete_applied_literal`
// function, so the index of the variable with the highest number of variables
// is returned from the variables in the current clauses.
Fn maximum_literal_number_index(const Vec<Vec<i32>>& clauses)->i32 {
  Map<i32, i32> frequency;
  for (Let& clause : clauses) {
    for (Let& literal : clause) {
      auto result = frequency.insert({literal_to_index(literal), 1});
      if (!result.second) {
        result.first->second++;
      }
    }
  }
  auto x = std::max_element(
      frequency.begin(), frequency.end(),
      [](Let& p1, Let& p2) { return p1.second > p2.second; }
  );
  return x->first;
}

// Delete variables from the clauses for which variable assignment has been
// determined.
Fn delete_set_literal(
    Vec<Vec<i32>>& clauses, const i32& index, const i32& set_val
)
    ->Status {
  for (auto itr1 = clauses.begin(); itr1 != clauses.end(); ++itr1) {
    for (auto itr2 = itr1->begin(); itr2 != itr1->end(); ++itr2) {
      // set_val -> unassigned(-1) -> always false
      // set_val -> true(0) -> value == index + 1
      // set_val -> false(1) -> value == -(index + 1)
      if (set_val >= 0
          && (set_val == 0 ? index + 1 : -1 * (index + 1)) == *itr2) {
        clauses.erase(itr1);
        --itr1; // reset iterator
        if (clauses.empty()) {
          return Status::satisfied;
        }
        break; // to the next clause
      } else if (index == literal_to_index(*itr2)) { // the literal with
                                                     // opposite polarity
        itr1->erase(itr2); // remove the literal from the clause
        --itr2; // reset iterator
        if (itr1->empty()) {
          // unsatisfiable currently
          return Status::unsatisfied;
        }
        break; // to the next clause
      }
    }
  }
  return Status::normal;
}

// unit resolution
Fn unit_propagate(Vec<Vec<i32>>& clauses, Vec<i32>& literals)->Status {
  bool unit_clause_found = true;
  while (unit_clause_found) {
    unit_clause_found = false;

    for (auto itr = clauses.begin(); itr != clauses.end(); ++itr) {
      if (itr->size() == 1) { // unit clause ({3}, {5}, ...)
        unit_clause_found = true;

        // 0 - if true, 1 - if false, set the literal
        literals[literal_to_index(*itr->begin())] = *itr->begin() < 0;

        const i32 index = literal_to_index(*itr->begin());
        const Status result =
            delete_set_literal(clauses, index, literals[index]);
        if (result == Status::satisfied || result == Status::unsatisfied) {
          return result;
        }
        // Since another unit clause may have been created by this deletion,
        // loop again from the beginning.
        break;
      } else if (itr->empty()) {
        // the formula is unsatisfiable in this branch
        return Status::unsatisfied;
      }
    }
  }
  return Status::normal;
}

// recursive DPLL algorithm
[[nodiscard]] Fn dpll(Vec<Vec<i32>>& clauses, Vec<i32>& literals)
    ->Result<Vec<i32>, String> {
  // NOLINTNEXTLINE(bugprone-branch-clone)
  if (clauses.empty()) {
    return Ok(to_assignments(literals));
  } else if (const Status result = unit_propagate(clauses, literals);
             result == Status::satisfied) {
    return Ok(to_assignments(literals));
  } else if (result == Status::unsatisfied) {
    return Err(
        "could not solve dependencies.\n"
        "detail: given SAT problem was unsatisfied."
    );
  }

  // The variable with the greatest frequency will be the next value assigned.
  const i32 i = maximum_literal_number_index(clauses);
  // need to apply twice, once true, the other false
  for (i32 j = 0; j < 2; ++j) {
    // copy the formula before recursive circulation
    Vec<i32> new_literals = literals;

    // if the number of literals with positive polarity are greater
    // cond ? positive : negative
    new_literals[i] =
        calc_literal_polarity(clauses, i + 1) > 0 ? j : (j + 1) % 2;

    // apply the change to all the clauses
    if (const Status result = delete_set_literal(clauses, i, new_literals[i]);
        result == Status::satisfied) {
      return Ok(to_assignments(new_literals));
    } else if (result == Status::unsatisfied) {
      // in this branch, return normally
      continue;
    }

    auto result = dpll(clauses, new_literals);
    if (result.is_ok()) {
      return result;
    }
  }
  return Err(
      "could not solve dependencies.\n"
      "detail: given SAT problem was unsatisfied."
  );
}

} // namespace poac::core::resolver::sat
