#ifndef POAC_CORE_SAT_HPP
#define POAC_CORE_SAT_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <numeric>
#include <cmath>

#include <boost/range/adaptor/indexed.hpp>


namespace poac::core::sat {
    enum class Sat : int {
        satisfied,   // when a satisfying assignment has been found
        unsatisfied, // when no satisfying assignment has been found after exhaustively searching
        normal,   // when no satisfying assignment has been found till now, and DPLL()
        // has exited normally
        completed // when the DPLL algorithm has completed execution; result has been found, exit recursion all the way
    };

    // vector to store the number of occurrences of each literal.
    // return an index with the most literal number and literal being -1.
    // frequency
//    void maximum_literal_number_index() { // TODO: literalsが-1のなかで，最大値を引き出す．
//
//    }

    int literal_to_index(int l) {
        return std::abs(l) - 1;
    }

    std::vector<int>
    to_assignments(const std::vector<int>& literals) {
        std::vector<int> assignments;
        for (const auto& l : literals | boost::adaptors::indexed()) {
            const int index = l.index() + 1;
            if (l.value() != -1) {
                assignments.emplace_back((l.value() % 2 == 0 ? 1 : -1) * index);
            }
            else { // for literals which can take either value, arbitrarily assign them to be true
                assignments.emplace_back(index);
            }
        }
        return assignments;
    }

    // vector to store the difference in number of occurrences with
    // positive and negative polarity of each literal
    template <
            template <class T, class=std::allocator<T>> typename TwoDim,
            template <class T, class=std::allocator<T>> typename OneDim,
            typename T, typename U
    >
    T calc_literal_polarity(const TwoDim<OneDim<T>>& rng, const U& i) {
        return std::accumulate(std::begin(rng), std::end(rng), static_cast<T>(0), [&](T accs, OneDim<T> xs) {
            return accs + std::accumulate(std::begin(xs), std::end(xs), static_cast<T>(0), [&](T acc, T x) {
                if (std::abs(x) == i) {
                    return x > 0 ? acc + 1 : acc - 1;
                }
                else {
                    return acc;
                }
            });
        });
    }

    // applies a value of a literal to all clauses in a given formula
    Sat apply_transform(std::vector<std::vector<int>>& clauses, const std::vector<int>& literals, const int& index) {
        // the value to apply, 0 - if true, 1 - if false
        const int value_to_apply = literals[index];

        for (auto itr1 = clauses.begin(); itr1 != clauses.end(); ++itr1) {
            for (auto itr2 = itr1->begin(); itr2 != itr1->end(); ++itr2) {
                // if this is true, then the literal appears with the same polarity as it
                // is being applied that is, if assigned true, it appears positive if
                // assigned false, it appears negative, in this clause hence, the clause
                // has now become true

                // value_to_apply -> unassigned(-1) -> always false
                // value_to_apply -> true(0) -> value == index + 1
                // value_to_apply -> false(1) -> value == -(index + 1)
                if (value_to_apply >= 0 && (value_to_apply == 0 ? index + 1 : -1 * (index + 1)) == *itr2) { // FIXME: これこんなにループする意味あるの？？？
                    clauses.erase(itr1); // remove the clause from the list
                    --itr1; // reset iterator
                    if (clauses.empty()) { // If all clauses have been removed, the formula is satisfied
                        return Sat::satisfied;
                    }
                    break; // move to the next clause
                }
                else if (literal_to_index(*itr2) == index) { // the literal appears with opposite polarity
                    itr1->erase(itr2); // remove the literal from the clause, as it is false in it
                    --itr2; // reset iterator
                    if (itr1->empty()) { // If the clause is empty, the formula is unsatisfiable currently
                        return Sat::unsatisfied;
                    }
                    break; // move to the next clause
                }
            }
        }
        // if reached here, the function is exiting normally
        return Sat::normal;
    }

    // performs unit propagation
    // function to perform unit resolution in a given formula
    Sat unit_propagate(std::vector<std::vector<int>>& clauses, std::vector<int>& literals, std::vector<int>& literal_frequency) {
        if (clauses.size() == 0) { // if the formula contains no clauses
            return Sat::satisfied; // it is vacuously satisfied
        }

        bool unit_clause_found = true; // stores whether the current iteration found a unit clause
        while (unit_clause_found) {
            unit_clause_found = false;

            for (auto itr = clauses.begin(); itr != clauses.end(); ++itr) {
                if (itr->size() == 1) { // if the size of a clause is 1, it is a unit clause
                    unit_clause_found = true;

                    // 0 - if true, 1 - if false, set the literal
                    literals[literal_to_index((*itr)[0])] = (*itr)[0] < 0; // FIXME: 0の場合を弾いていない -> これが，-1でなければ，無効化じゃない
                    // once assigned, reset the frequency to mark it closed
                    literal_frequency[literal_to_index((*itr)[0])] = -1; // FIXME: ここはただの無効化
                    // apply this change through f
                    Sat result = apply_transform(clauses, literals, literal_to_index((*itr)[0]));

                    // if this caused the formula to be either satisfied or unsatisfied,
                    // return the result flag
                    if (result == Sat::satisfied || result == Sat::unsatisfied) {
                        return result;
                    }
                    break; // exit the loop to check for another unit clause from the start
                }
                else if (itr->empty()) { // if a given clause is empty
                    return Sat::unsatisfied; // the formula is unsatisfiable in this branch
                }
            }
        }
        return Sat::normal; // if reached here, the unit resolution ended normally
    }

    // perform the recursive DPLL on a given formula
    std::pair<Sat, std::vector<int>>
    DPLL(std::vector<std::vector<int>>& clauses, std::vector<int>& literals, std::vector<int> literal_frequency) { // FIXME: コピーでOK？？
        Sat result = unit_propagate(clauses, literals, literal_frequency); // perform unit propagation on the formula
        if (result == Sat::satisfied) { // if formula satisfied, show result and return
            return { Sat::completed, to_assignments(literals) };
        }
        else if (result == Sat::unsatisfied) { // if formula not satisfied in this
            // branch, return normally
            return { Sat::normal, {} };
        }

        // find the variable with maximum frequency in f, which will be the next to be
        // assigned a value already assigned variables have this field reset to -1 in
        // order to ignore them
        const auto first = literal_frequency.begin();
        // TODO: literalの最大個数が最大の要素のindex -> -1を代入して，無効化している -> 代わりになにかできないか？
        const int i = static_cast<int>(std::distance(first, std::max_element(first, literal_frequency.end())));
        // need to apply twice, once true, the other false
        for (int j = 0; j < 2; j++) {
            // copy the formula before recursing // FIXME: コピーでOK？？？
            std::vector<int> new_literals = literals;
            std::vector<int> new_literal_frequency = literal_frequency;

            if (calc_literal_polarity(clauses, i + 1) > 0) { // if the number of literals with positive polarity are greater
                new_literals[i] = j; // assign positive first
            }
            else {
                new_literals[i] = (j + 1) % 2; // assign negative first
            }
            new_literal_frequency[i] = -1; // reset the frequency to -1 to ignore in the future // FIXME: ここもただの無効化
            Sat transform_result = apply_transform(clauses, new_literals, i); // apply the change to all the clauses
            if (transform_result == Sat::satisfied) { // if formula satisfied, show result and return
                return { Sat::completed, to_assignments(new_literals) };
            }
            else if (transform_result == Sat::unsatisfied) { // if formula not satisfied
                // in this branch, return normally
                continue;
            }

            // recursively call DPLL on the new formula
            const auto [dpll_result, assignments] = DPLL(clauses, new_literals, new_literal_frequency);
            if (dpll_result == Sat::completed) { // propagate the result, if completed
                return { dpll_result, assignments };
            }
        }
        // if the control reaches here, the function has returned normally
        return { Sat::normal, {} };
    }

    void init_literal_frequency(
            std::vector<int>& literal_frequency,
            const std::vector<std::vector<int>>& receive_clauses,
            const unsigned long& variables)
    {
        literal_frequency.resize(variables, 0);
        // iterate over the clauses
        int i = 0;
        for (const auto& clause : receive_clauses) {
            for (const auto& literal : clause) {
                if (literal > 0) {
                    ++(literal_frequency[literal - 1]);
                }
                else if (literal < 0) {
                    ++(literal_frequency[-1 - literal]);
                }
                else { // To next clause
                    break;
                }
            }
            ++i;
        }
    }

    std::pair<Sat, std::vector<int>>
    solve(std::vector<std::vector<int>> clauses, const unsigned long& variables) {
        std::vector<int> literal_frequency;
        init_literal_frequency(literal_frequency, clauses, variables);

        // indexに対応するリテラル値の割り当て状態を表現する
        // a vector that stores the value assigned to each variable, where
        // -1 - unassigned
        // 0 - true
        // 1 - false
        std::vector<int> literals(variables, -1);

        // The return value is Sat::completed or Sat::normal and if it is Sat::normal,
        //  the formula could not be satisfied in any branch, so it is unsatisfiable.
        return DPLL(clauses, literals, literal_frequency);
    }
} // end namespace
#endif // !POAC_CORE_SAT_HPP
