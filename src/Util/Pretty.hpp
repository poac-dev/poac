module;

// std
#include <array>
#include <chrono>

// external
#include <boost/algorithm/string.hpp>

export module poac.util.pretty;

import poac.util.format;
import poac.util.rustify;

export namespace poac::util::pretty {

auto to_time(const f64& total_seconds) -> String {
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

inline auto to_time(const std::chrono::seconds& s) -> String {
  return to_time(s.count());
}

inline auto to_time(const String& s) -> String { return to_time(std::stod(s)); }

inline constexpr std::array<StringRef, 9> SIZE_SUFFIXES = {
    "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

auto to_byte(f64 bytes) -> String {
  int index = 0;
  while (bytes >= 1000.0) {
    bytes /= 1024.0;
    ++index;
  }
  return format("{:.2f}{}", bytes, SIZE_SUFFIXES.at(index));
}

// If string size is over specified number of characters and it can be clipped,
//  display an ellipsis (...).
inline auto clip_string(const String& s, const usize& n) -> String {
  return s.size() <= n ? s : s.substr(0, n) + "...";
}

// This function does not break long words and break on hyphens.
// This assumes that space size is one (` `), not two (`  `).
// textwrap(s, 15) =>
// This function
// does not break
// long words and
// break on
// hyphens.
auto textwrap(const String& text, usize width) -> Vec<String> {
  Vec<String> split_texts;
  boost::split(split_texts, text, boost::is_space());

  Vec<String> wrapped_texts;
  String consuming_text;
  for (const auto& st : split_texts) {
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
