// external
#include <boost/algorithm/string.hpp>

// internal
#include "poac/util/format.hpp"
#include "poac/util/pretty.hpp"
#include "poac/util/rustify.hpp"

namespace poac::util::pretty {

Fn to_time(const f64& total_seconds)->String {
  if (total_seconds <= 1.0) {
    return format("{:.2f}s", total_seconds);
  }

  String res;
  const u64 total_secs = static_cast<u64>(total_seconds);
  if (const u64 days = total_secs / 60 / 60 / 24; days > 0) {
    res += std::to_string(days) + "d ";
  }
  if (const u64 hours = (total_secs / 60 / 60) % 24; hours > 0) {
    res += std::to_string(hours) + "h ";
  }
  if (const u64 minutes = (total_secs / 60) % 60; minutes > 0) {
    res += std::to_string(minutes) + "m ";
  }
  const u64 seconds = total_secs % 60;
  res += std::to_string(seconds) + "s";
  return res;
}

Fn to_byte(f64 bytes)->String {
  int index = 0;
  while (bytes >= 1000.0) {
    bytes /= 1024.0;
    ++index;
  }
  return format("{:.2f}{}", bytes, SIZE_SUFFIXES.at(index));
}

// This function does not break long words and break on hyphens.
// This assumes that space size is one (` `), not two (`  `).
// textwrap(s, 15) =>
// This function
// does not break
// long words and
// break on
// hyphens.
Fn textwrap(const String& text, usize width)->Vec<String> {
  Vec<String> split_texts;
  boost::split(split_texts, text, boost::is_space());

  Vec<String> wrapped_texts;
  String consuming_text;
  for (Let& st : split_texts) {
    if (consuming_text.size() + st.size() < width) {
      consuming_text +=
          consuming_text.empty() ? st : " " + st; // assumes space size is one
    } else {
      wrapped_texts.emplace_back(consuming_text);
      consuming_text = st;
    }
  }
  if (!consuming_text.empty()) {
    wrapped_texts.emplace_back(consuming_text);
  }
  return wrapped_texts;
}

} // namespace poac::util::pretty
