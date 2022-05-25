// internal
#include "poac/util/semver/interval.hpp"

namespace semver {

bool
BoundedInterval::satisfies_impl(std::string_view v) const {
  if (left_comp_op == ">") {
    if (right_comp_op == "<") {
      return (parse(v) > left_version) && (parse(v) < right_version);
    } else if (right_comp_op == "<=") {
      return (parse(v) > left_version) && (parse(v) <= right_version);
    }
  } else if (left_comp_op == ">=") {
    if (right_comp_op == "<") {
      return (parse(v) >= left_version) && (parse(v) < right_version);
    } else if (right_comp_op == "<=") {
      return (parse(v) >= left_version) && (parse(v) <= right_version);
    }
  } else if (left_comp_op == "<") {
    if (right_comp_op == ">") {
      return (parse(v) < left_version) && (parse(v) > right_version);
    } else if (right_comp_op == ">=") {
      return (parse(v) < left_version) && (parse(v) >= right_version);
    }
  } else if (left_comp_op == "<=") {
    if (right_comp_op == ">") {
      return (parse(v) <= left_version) && (parse(v) > right_version);
    } else if (right_comp_op == ">=") {
      return (parse(v) <= left_version) && (parse(v) >= right_version);
    }
  }
  return false;
}

// e.g. `>0.1.3 and >=0.3.2`, `<0.1.3 and <0.3.2`
std::optional<std::string>
BoundedInterval::is_wasteful_comparison_operation(
) const { // TODO(ken-matsui): noexcept
  if ((left_comp_op == "<" || left_comp_op == "<=") &&
      (right_comp_op == "<" || right_comp_op == "<=")) {
    // Prioritize the larger version
    if (parse(left_version) > right_version) {
      return "Did you mean " + left_comp_op + left_version + " ?";
    } else {
      return "Did you mean " + right_comp_op + right_version + " ?";
    }
  } else if ((left_comp_op == ">" || left_comp_op == ">=") && (right_comp_op == ">" || right_comp_op == ">=")) {
    // Prioritize the smaller version
    if (parse(left_version) < right_version) {
      return "Did you mean " + left_comp_op + left_version + " ?";
    } else {
      return "Did you mean " + right_comp_op + right_version + " ?";
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
BoundedInterval::is_bounded_interval() const { // TODO(ken-matsui): noexcept
  if (parse(left_version) < right_version) {
    if ((left_comp_op == "<" || left_comp_op == "<=") &&
        (right_comp_op == ">" || right_comp_op == ">=")) {
      return "In this case of interval specification using `and` +\n"
             " it is necessary to be a bounded interval.\n"
             "Please specify as in the following example:\n"
             "e.g. `" +
             right_comp_op + left_version + " and " + left_comp_op +
             right_version + "`";
    }
  } else if (parse(left_version) > right_version) {
    if ((left_comp_op == ">" || left_comp_op == ">=") &&
        (right_comp_op == "<" || right_comp_op == "<=")) {
      return "In this case of interval specification using `and` +\n"
             " it is necessary to be a bounded interval.\n"
             "Please specify as in the following example:\n"
             "e.g. `" +
             left_comp_op + right_version + " and " + right_comp_op +
             left_version + "`";
    }
  }
  return std::nullopt;
}

BoundedInterval::BoundedInterval(
    const std::smatch& match, std::string_view interval
)
    : left_comp_op{match[2].str()}, right_comp_op{match[9].str()},
      left_version{make_version<3, 7>(match)},
      right_version{make_version<10, 14>(match)} {
  if (const auto error = is_wasteful_comparison_operation()) {
    throw redundant_interval_error(std::string(interval), error.value());
  }
  if (const auto error = is_bounded_interval()) {
    throw strange_interval_error(std::string(interval), error.value());
  }
}

// >2.3.0, 1.0.0, <=1.2.3-alpha, ...
bool
ClosedUnboundedInterval::satisfies_impl(std::string_view v) const {
  if (comp_op == ">") {
    return parse(v) > version_str;
  } else if (comp_op == ">=") {
    return parse(v) >= version_str;
  } else if (comp_op == "<") {
    return parse(v) < version_str;
  } else if (comp_op == "<=") {
    return parse(v) <= version_str;
  }
  return false;
}

Interval::IntervalClass
Interval::get_interval_class() const {
  std::smatch match;
  if (interval_match(match, CLOSED_UNBOUNDED_INTERVAL)) {
    const std::string comp_op = match[2].str();
    const std::string version_str = make_version<3, 5>(match);
    return comp_op.empty() ? make_interval_class<ExactVersion>(version_str)
                           : make_interval_class<ClosedUnboundedInterval>(
                                 comp_op, version_str
                             );
  } else if (interval_match(match, BOUNDED_INTERVAL)) {
    return BoundedInterval{match, interval};
  }
  throw invalid_interval_error(
      interval,
      "Comparison operators:\n"
      "  >, >=, <, <=\n"
      "Logical operator:\n"
      "  and\n"
      "The following example is the meaning for equals:\n"
      "  example: \"1.2.0\""
  );
}

} // end namespace semver
