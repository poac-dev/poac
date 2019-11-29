#ifndef SEMVER_INTERVAL_HPP
#define SEMVER_INTERVAL_HPP

#include <string>
#include <regex>
#include <stdexcept>
#include <optional>

#include <poac/util/semver/version.hpp>
#include <poac/util/semver/comparison.hpp>

namespace semver {
    enum class IntervalMode {
        Equal,
        ClosedUnbounded,
        Bounded
    };

    class Interval {
    public:
        const std::string name;
        const std::string interval;

        std::string comp_op;
        std::string version_str;

        std::string first_comp_op;
        std::string second_comp_op;
        std::string first_version;
        std::string second_version;

        IntervalMode mode;

        explicit Interval(const std::string& n, const std::string& i) : name(n), interval(i) {
            std::smatch match;
            if (std::regex_match(interval, match, std::regex(CLOSED_UNBOUNDED_INTERVAL))) {
                comp_op = match[2].str();
                version_str = match[3].str() + "." + match[4].str() + "." + match[5].str();

                // Check
                if (comp_op.empty()) { // equal
                    mode = IntervalMode::Equal;
                }
                else {
                    mode = IntervalMode::ClosedUnbounded;
                }
            }
            else if (std::regex_match(interval, match, std::regex(BOUNDED_INTERVAL))) {
                first_comp_op = match[2].str();
                second_comp_op = match[9].str();

                first_version = match[3].str() + "." + match[4].str() + "." + match[5].str();
                first_version += match[6].matched ? ("-" + match[6].str()) : "";
                first_version += match[7].matched ? ("+" + match[7].str()) : "";

                second_version = match[10].str() + "." + match[11].str() + "." + match[12].str();
                second_version += match[13].matched ? ("-" + match[13].str()) : "";
                second_version += match[14].matched ? ("+" + match[14].str()) : "";

                // Checks
                if (const auto error = is_wasteful_comparison_operation()) {
                    throw std::range_error(error.value());
                }
                if (const auto error = is_bounded_interval()) {
                    throw std::range_error(error.value());
                }
                mode = IntervalMode::Bounded;
            }
            else {
                throw std::invalid_argument(
                        "`" + name + ": " + interval + "` is invalid expression.\n"
                        "Comparison operators:\n"
                        "  >, >=, <, <=\n"
                        "Logical operator:\n"
                        "  and\n"
                        "The following example is the meaning for equals:\n"
                        "  example: \"1.2.0\"");
            }
        }

        bool satisfies(const std::string& version) const {
            switch (mode) {
                case IntervalMode::Equal:
                    return version == interval;
                case IntervalMode::ClosedUnbounded:
                    return satisfies_closed_unbounded_interval(version);
                case IntervalMode::Bounded:
                    return satisfies_bounded_interval(version);
                default:
                    throw std::logic_error(
                            "To access out of range of the "
                            "enumeration values is undefined behavior.");
            }
        }

    private:
        // >2.3.0, 1.0.0, <=1.2.3-alpha, ...
        bool satisfies_closed_unbounded_interval(const std::string& v) const {
            if (comp_op == ">") {
                return Version(v) > version_str;
            }
            else if (comp_op == ">=") {
                return Version(v) >= version_str;
            }
            else if (comp_op == "<") {
                return Version(v) < version_str;
            }
            else if (comp_op == "<=") {
                return Version(v) <= version_str;
            }
            return false;
        }

        // e.g. `>0.1.3 and >=0.3.2`, `<0.1.3 and <0.3.2`
        std::optional<std::string>
        is_wasteful_comparison_operation() const { // TODO: noexcept
            if ((first_comp_op == "<" || first_comp_op == "<=")
                && (second_comp_op == "<" || second_comp_op == "<="))
            {
                if (Version(first_version) > second_version) { // Prioritize the larger version
                    return "`" + name + ": " + interval + "` is invalid expression.\n"
                           "Did you mean " + first_comp_op + first_version + " ?";
                }
                else {
                    return "`" + name + ": " + interval + "` is invalid expression.\n"
                           "Did you mean " + second_comp_op + second_version + " ?";
                }
            }
            else if ((first_comp_op == ">" || first_comp_op == ">=")
                     && (second_comp_op == ">" || second_comp_op == ">="))
            {
                if (Version(first_version) < second_version) { // Prioritize the smaller version
                    return "`" + name + ": " + interval + "` is invalid expression.\n"
                           "Did you mean " + first_comp_op + first_version + " ?";
                }
                else {
                    return "`" + name + ": " + interval + "` is invalid expression.\n"
                           "Did you mean " + second_comp_op + second_version + " ?";
                }
            }
            return std::nullopt;
        }

        // Check if it is bounded interval
        //  (If it is unbounded, throw error)
        // (1, 6) => open bounded interval => OK!
        // [1, 6] => closed bounded interval => OK!
        // [a, ∞) => closed unbounded interval => one_exp
        // (-∞, ∞) => closed unbounded interval => ERR!
        // e.g. <0.1.1 and >=0.3.2
        std::optional<std::string>
        is_bounded_interval() const { // TODO: noexcept
            if (Version(first_version) < second_version) {
                if ((first_comp_op == "<" || first_comp_op == "<=")
                    && (second_comp_op == ">" || second_comp_op == ">="))
                {
                    return "`" + name + ": " + interval + "` is strange.\n"
                           "In this case of interval specification using `and` +\n"
                           " it is necessary to be a bounded interval.\n"
                           "Please specify as in the following example:\n"
                           "e.g. `" + second_comp_op + first_version + " and " +
                           first_comp_op + second_version + "`";
                }
            }
            else if (Version(first_version) > second_version) {
                if ((first_comp_op == ">" || first_comp_op == ">=")
                    && (second_comp_op == "<" || second_comp_op == "<="))
                {
                    return "`" + name + ": " + interval + "` is strange.\n"
                           "In this case of interval specification using `and` +\n"
                           " it is necessary to be a bounded interval.\n"
                           "Please specify as in the following example:\n"
                           "e.g. `" + first_comp_op + second_version + " and " +
                           second_comp_op + first_version + "`";
                }
            }
            return std::nullopt;
        }

        bool satisfies_bounded_interval(const std::string& v) const {
            if (first_comp_op == ">") {
                if (second_comp_op == "<") {
                    return (Version(v) > first_version) && (Version(v) < second_version);
                }
                else if (second_comp_op == "<=") {
                    return (Version(v) > first_version) && (Version(v) <= second_version);
                }
            }
            else if (first_comp_op == ">=") {
                if (second_comp_op == "<") {
                    return (Version(v) >= first_version) && (Version(v) < second_version);
                }
                else if (second_comp_op == "<=") {
                    return (Version(v) >= first_version) && (Version(v) <= second_version);
                }
            }
            else if (first_comp_op == "<") {
                if (second_comp_op == ">") {
                    return (Version(v) < first_version) && (Version(v) > second_version);
                }
                else if (second_comp_op == ">=") {
                    return (Version(v) < first_version) && (Version(v) >= second_version);
                }
            }
            else if (first_comp_op == "<=") {
                if (second_comp_op == ">") {
                    return (Version(v) <= first_version) && (Version(v) > second_version);
                }
                else if (second_comp_op == ">=") {
                    return (Version(v) <= first_version) && (Version(v) >= second_version);
                }
            }
            return false;
        }
    };
} // end namespace semver

#endif // !SEMVER_INTERVAL_HPP
