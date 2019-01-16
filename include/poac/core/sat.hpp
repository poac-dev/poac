#ifndef POAC_CORE_SAT_HPP
#define POAC_CORE_SAT_HPP

#include <iostream>
#include <vector>
#include <stack>
#include <utility>
#include <string>


namespace poac::core::sat {
    enum class Sat : int {
        satisfied,   // when a satisfying assignment has been found
        unsatisfied, // when no satisfying assignment has been found after exhaustively searching
        normal,   // when no satisfying assignment has been found till now, and DPLL()
        // has exited normally
        completed // when the DPLL algorithm has completed execution
    };

    class Formula {
    public:
        // a vector that stores the value assigned to each variable, where
        // -1 - unassigned
        // 0 - true
        // 1 - false
        std::vector<int> literals;
        std::vector<int> literal_frequency; // vector to store the number of occurrences of
        // each literal

        // vector to store the difference in number of occurrences with
        // positive and negative polarity of each literal
        std::vector<int> literal_polarity;

        // vector to store the clauses
        // for each clauses, if the variable n is of positive polarity, then 2n is
        // stored if the variable n is of negative polarity, then 2n+1 is stored here,
        // n is assumed to be zero indexed
        std::vector<std::vector<int>> clauses;
        Formula(const std::vector<std::vector<int>>& recieve_clauses, const unsigned long variables) {
            // the number of clauses in the formula
            unsigned long clause_count = recieve_clauses.size();

            // set the vectors to their appropriate sizes and initial values
            literals.resize(variables, -1);
            clauses.resize(clause_count);
            literal_frequency.resize(variables, 0);
            literal_polarity.resize(variables, 0);

            // iterate over the clauses
            int i = 0;
            for (const auto& cls : recieve_clauses) {
                for (const auto& literal : cls) {
                    if (literal > 0) {
                        clauses[i].push_back(2 * (literal - 1)); // 2n -> 0, 2, 4, 6, ...
                        // increment frequency and polarity of the literal
                        literal_frequency[literal - 1]++;
                        literal_polarity[literal - 1]++;
                    }
                    else if (literal < 0) {
                        clauses[i].push_back(2 * ((-1) * literal - 1) + 1); // 2n+1 -> 1, 3, 5, ...
                        // increment frequency and decrement polarity of the literal
                        literal_frequency[-1 - literal]++;
                        literal_polarity[-1 - literal]--;
                    }
                    else { // To next clause
                        break;
                    }
                }
                ++i;
            }
        }

        // copy constructor for copying a formula - each member is copied over
        Formula(const Formula& f) {
            literals = f.literals;
            clauses = f.clauses;
            literal_frequency = f.literal_frequency;
            literal_polarity = f.literal_polarity;
        }

        Formula(Formula&&) = delete;
        Formula& operator=(Formula&&) = delete;
    };

    /*
     * displays the result
     * function to display the result of the solver
     * arguments: f - the formula when it was satisfied or shown to be unsatisfiable
     *            result - the result flag, a member of the Cat enum
     */
    void show_result(const Formula& f, const Sat& result) {
        if (result == Sat::satisfied) { // if the formula is satisfiable
            std::cout << "SAT" << std::endl;
            for (int i = 0; i < static_cast<int>(f.literals.size()); i++) {
                if (i != 0) {
                    std::cout << " ";
                }

                if (f.literals[i] != -1) {
                    std::cout << pow(-1, f.literals[i]) * (i + 1);
                }
                else { // for literals which can take either value, arbitrarily assign
                    // them to be true
                    std::cout << (i + 1);
                }
            }
            std::cout << " 0" << std::endl;
        }
        else { // if the formula is unsatisfiable
            std::cout << "UNSAT" << std::endl;
        }
    }

    /*
     * displays the result
     * function to display the result of the solver
     * arguments: f - the formula when it was satisfied or shown to be unsatisfiable
     *            result - the result flag, a member of the Cat enum
     */
    std::vector<int> to_result(const Formula& f) {
        std::vector<int> result;

        for (int i = 0; i < static_cast<int>(f.literals.size()); i++) {
            if (f.literals[i] != -1) {
                result.push_back( pow(-1, f.literals[i]) * (i + 1) );
            }
            else { // for literals which can take either value, arbitrarily assign
                // them to be true
                result.push_back(i + 1);
            }
        }

        return result;
    }

    /*
     * applies the value of the literal in every clause
     * applies a value of a literal to all clauses in a given formula
     * arguments: f - the formula to apply on
     *            literal_to_apply - the literal which has just been set
     * return value: int - the return status flag, a member of the Cat enum
     *               Cat::satisfied - the formula has been satisfied
     *               Cat::unsatisfied - the formula can no longer be satisfied
     *               Cat::normal - normal exit
     */
    Sat apply_transform(Formula& f, const int literal_to_apply) {
        int value_to_apply = f.literals[literal_to_apply]; // the value to apply, 0 -
        // if true, 1 - if false
        // iterate over the clauses in f
        for (int i = 0; i < static_cast<int>(f.clauses.size()); i++) {
            // iterate over the variables in the clause
            for (int j = 0; j < static_cast<int>(f.clauses[i].size()); j++) {
                // if this is true, then the literal appears with the same polarity as it
                // is being applied that is, if assigned true, it appears positive if
                // assigned false, it appears negative, in this clause hence, the clause
                // has now become true
                if ((2 * literal_to_apply + value_to_apply) == f.clauses[i][j]) {
                    f.clauses.erase(f.clauses.begin() + i); // remove the clause from the list // TODO: この辺りstack使った方がよく無い？？？？
                    i--;                // reset iterator
                    if (f.clauses.empty()) { // if all clauses have been removed, the formula is satisfied
                        return Sat::satisfied;
                    }
                    break; // move to the next clause
                }
                else if (f.clauses[i][j] / 2 == literal_to_apply) { // the literal appears with opposite polarity
                    f.clauses[i].erase(f.clauses[i].begin() + j); // remove the literal from the clause, as it is false in it
                    j--;    // reset the iterator
                    if (f.clauses[i].empty()) { // if the clause is empty, the formula is unsatisfiable currently
                        return Sat::unsatisfied;
                    }
                    break; // move to the next clause
                }
            }
        }
        // if reached here, the function is exiting normally
        return Sat::normal;
    }

    /*
     * performs unit propagation
     * function to perform unit resolution in a given formula
     * arguments: f - the formula to perform unit resolution on
     * return value: int - the status of the solver after unit resolution, a member
     * of the Cat enum Cat::satisfied - the formula has been satisfied
     *               Cat::unsatisfied - the formula can no longer be satisfied
     *               Cat::normal - normal exit
     */
    Sat unit_propagate(Formula& f) {
        if (f.clauses.size() == 0) { // if the formula contains no clauses
            return Sat::satisfied; // it is vacuously satisfied
        }

        bool unit_clause_found = true; // stores whether the current iteration found a unit clause
        while (unit_clause_found) {
            unit_clause_found = false;

            // iterate over the clauses in f
            for (int i = 0; i < static_cast<int>(f.clauses.size()); i++) {
                if (f.clauses[i].size() == 1) { // if the size of a clause is 1, it is a unit clause
                    unit_clause_found = true;
                    f.literals[f.clauses[i][0] / 2] = f.clauses[i][0] % 2; // 0 - if true, 1 - if false, set the literal
                    f.literal_frequency[f.clauses[i][0] / 2] = -1; // once assigned, reset the frequency to mark it closed
                    Sat result = apply_transform(f, f.clauses[i][0] / 2); // apply this change through f
                    // if this caused the formula to be either satisfied or unsatisfied,
                    // return the result flag
                    if (result == Sat::satisfied || result == Sat::unsatisfied) {
                        return result;
                    }
                    break; // exit the loop to check for another unit clause from the start
                }
                else if (f.clauses[i].size() == 0) { // if a given clause is empty
                    return Sat::unsatisfied; // the formula is unsatisfiable in this branch
                }
            }
        }
        return Sat::normal; // if reached here, the unit resolution ended normally
    }

    /*
     * performs DPLL recursively
     * function to perform the recursive DPLL on a given formula
     * argument: f - the formula to perform DPLL on
     * return value: int - the return status flag, a member of the Cat enum
     *               Cat::normal - exited normally
     *               Cat::completed - result has been found, exit recursion all the
     * way
     */
    std::pair<Sat, std::vector<int>> DPLL(Formula f) {
        Sat result = unit_propagate(f); // perform unit propagation on the formula
        if (result == Sat::satisfied) { // if formula satisfied, show result and return
            const auto result_vec = to_result(f);
            return std::make_pair(Sat::completed, result_vec);
        }
        else if (result == Sat::unsatisfied) { // if formula not satisfied in this
            // branch, return normally
            return std::make_pair(Sat::normal, std::vector<int>{});
        }

        // find the variable with maximum frequency in f, which will be the next to be
        // assigned a value already assigned variables have this field reset to -1 in
        // order to ignore them
        const auto first = f.literal_frequency.begin();
        const auto last = f.literal_frequency.end();
        int i = static_cast<int>(std::distance(first, std::max_element(first, last)));
        // need to apply twice, once true, the other false
        for (int j = 0; j < 2; j++) {
            Formula new_f = f; // copy the formula before recursing
            if (new_f.literal_polarity[i] > 0) { // if the number of literals with positive polarity are greater
                new_f.literals[i] = j; // assign positive first
            }
            else {                  // if not
                new_f.literals[i] = (j + 1) % 2; // assign negative first
            }
            new_f.literal_frequency[i] = -1; // reset the frequency to -1 to ignore in the future
            Sat transform_result = apply_transform(new_f, i); // apply the change to all the clauses
            if (transform_result == Sat::satisfied) { // if formula satisfied, show result and return
                const auto result_vec = to_result(new_f);
                return std::make_pair(Sat::completed, result_vec);
            }
            else if (transform_result == Sat::unsatisfied) { // if formula not satisfied
                // in this branch, return
                // normally
                continue;
            }

            const auto [dpll_result, result_vec] = DPLL(new_f); // recursively call DPLL on the new formula
            if (dpll_result == Sat::completed) { // propagate the result, if completed
                return std::make_pair(dpll_result, result_vec);
            }
        }
        // if the control reaches here, the function has returned normally
        return std::make_pair(Sat::normal, std::vector<int>{});
    }

    std::pair<Sat, std::vector<int>> solve(const Formula& f) {
        return DPLL(f);
//        const auto [result, result_vec] = DPLL(f); // final result of DPLL on the original formula
        // if normal return till the end, then the formula could not be satisfied in
        // any branch, so it is unsatisfiable
//        if (result == Sat::normal) {
//            show_result(f, Sat::unsatisfied); // the argument formula is a dummy
            // here, the result is UNSAT
//        }
    }
} // end namespace
#endif // !POAC_CORE_SAT_HPP
