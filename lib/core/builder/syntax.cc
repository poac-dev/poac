// external
#include <boost/regex.hpp>

// internal
#include "poac/core/builder/syntax.hpp"
#include "poac/util/pretty.hpp"

namespace poac::core::builder::syntax {

/// Expand a string containing $vars as Ninja would.
///
/// Note: doesn't handle the full Ninja variable syntax, but it's enough
/// to make configure.py's use of it work.
String
expand(const String& text, const Variables& vars, const Variables& local_vars) {
  const auto exp = [&](const boost::smatch& m) {
    const String var = m[1].str();
    if (var == "$") {
      return "$"s;
    }
    return local_vars.contains(var) ? local_vars.at(var)
           : vars.contains(var)     ? vars.at(var)
                                    : ""s;
  };
  return boost::regex_replace(text, boost::regex("\\$(\\$|\\w*)"), exp);
}

/// ref: https://stackoverflow.com/a/46379136
String
operator*(const String& s, usize n) {
  String result;
  result.reserve(s.size() * n);
  for (usize i = 0; i < n; ++i) {
    result += s;
  }
  return result;
}

/// Returns the number of '$' characters right in front of s[i].
usize
Writer::count_dollars_before_index(StringRef s, usize i) const {
  usize dollar_count = 0;
  usize dollar_index = i - 1;
  while (dollar_index > 0 && s[dollar_index] == '$') {
    dollar_count += 1;
    dollar_index -= 1;
  }
  return dollar_count;
}

/// Write 'text' word-wrapped at self.width characters.
void
Writer::_line(String text, usize indent) {
  String leading_space = String("  ") * indent;

  while (leading_space.length() + text.length() > width) {
    // The text is too wide; wrap if possible.

    // Find the rightmost space that would obey our width constraint and
    // that's not an escaped space.
    std::int32_t available_space =
        width - leading_space.length() - 2; // " $".length() == 2
    std::int32_t space = available_space;
    do {
      space = text.rfind(' ', space);
    } while (!(space < 0 || count_dollars_before_index(text, space) % 2 == 0));

    if (space < 0) {
      // No such space; just use the first unescaped space we can find.
      space = available_space - 1;
      do {
        space = text.find(' ', space + 1);
      } while (!(space < 0 || count_dollars_before_index(text, space) % 2 == 0)
      );
    }
    if (space < 0) {
      // Give up on breaking.
      break;
    }

    output << leading_space + text.substr(0, space) + " $\n";
    text = text.substr(space + 1);

    // Subsequent lines are continuations, so indent them.
    leading_space = String("  ") * (indent + 2);
  }
  output << leading_space + text + '\n';
}

void
Writer::comment(const String& text) {
  for (const String& line : util::pretty::textwrap(text, width - 2)) {
    output << "# " + line + '\n';
  }
}

void
Writer::variable(StringRef key, StringRef value, usize indent) {
  if (value.empty()) {
    return;
  }
  _line(format("{} = {}", key, value), indent);
}

void
Writer::rule(StringRef name, StringRef command, const RuleSet& rule_set) {
  _line(format("rule {}", name));
  variable("command", command, 1);
  if (rule_set.description.has_value()) {
    variable("description", rule_set.description.value(), 1);
  }
  if (rule_set.depfile.has_value()) {
    variable("depfile", rule_set.depfile.value(), 1);
  }
  if (rule_set.generator) {
    variable("generator", "1", 1);
  }
  if (rule_set.pool.has_value()) {
    variable("pool", rule_set.pool.value(), 1);
  }
  if (rule_set.restat) {
    variable("restat", "1", 1);
  }
  if (rule_set.rspfile.has_value()) {
    variable("rspfile", rule_set.rspfile.value(), 1);
  }
  if (rule_set.rspfile_content.has_value()) {
    variable("rspfile_content", rule_set.rspfile_content.value(), 1);
  }
  if (rule_set.deps.has_value()) {
    variable("deps", rule_set.deps.value(), 1);
  }
}

Vec<String>
Writer::build(
    const Vec<String>& outputs, StringRef rule, const BuildSet& build_set
) {
  Vec<String> out_outputs;
  for (const String& o : outputs) {
    out_outputs.emplace_back(escape_path(o).string());
  }

  Vec<String> all_inputs;
  if (build_set.inputs.has_value()) {
    for (const String& i : build_set.inputs.value()) {
      all_inputs.emplace_back(escape_path(i).string());
    }
  }

  if (build_set.implicit.has_value()) {
    Vec<String> implicit;
    for (const Path& i : build_set.implicit.value()) {
      implicit.emplace_back(escape_path(i).string());
    }
    all_inputs.emplace_back("|");
    boost::push_back(all_inputs, implicit);
  }
  if (build_set.order_only.has_value()) {
    Vec<String> order_only;
    for (const Path& o : build_set.order_only.value()) {
      order_only.emplace_back(escape_path(o).string());
    }
    all_inputs.emplace_back("||");
    boost::push_back(all_inputs, order_only);
  }
  if (build_set.implicit_outputs.has_value()) {
    Vec<String> implicit_outputs;
    for (const Path& i : build_set.implicit_outputs.value()) {
      implicit_outputs.emplace_back(escape_path(i).string());
    }
    out_outputs.emplace_back("|");
    boost::push_back(out_outputs, implicit_outputs);
  }

  _line(format(
      "build {}: {} {}", boost::algorithm::join(out_outputs, " "), rule,
      boost::algorithm::join(all_inputs, " ")
  ));

  if (build_set.pool.has_value()) {
    _line(format("  pool = {}", build_set.pool.value()));
  }
  if (build_set.dyndep.has_value()) {
    _line(format("  dyndep = {}", build_set.dyndep.value()));
  }

  if (build_set.variables.has_value()) {
    for (const auto& [key, val] : build_set.variables.value()) {
      variable(key, val, 1);
    }
  }

  return outputs;
}

} // namespace poac::core::builder::syntax
