#ifndef POAC_UTIL_PRETTY_HPP_
#define POAC_UTIL_PRETTY_HPP_

// std
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <utility>

// external
#include <boost/algorithm/string.hpp>

// internal
#include "poac/poac.hh"

namespace poac::util::pretty {

String
to_time(const f64& total_seconds) {
  if (total_seconds <= 1.0) {
    return format("{:.2f}s", total_seconds);
  }

  String res;
  const auto total_secs = static_cast<u64>(total_seconds);
  if (const auto days = total_secs / 60 / 60 / 24; days > 0) {
    res += std::to_string(days) + "d ";
  }
  if (const auto hours = (total_secs / 60 / 60) % 24; hours > 0) {
    res += std::to_string(hours) + "h ";
  }
  if (const auto minutes = (total_secs / 60) % 60; minutes > 0) {
    res += std::to_string(minutes) + "m ";
  }
  const auto seconds = total_secs % 60;
  res += std::to_string(seconds) + "s";
  return res;
}

inline String
to_time(const std::chrono::seconds& s) {
  return to_time(s.count());
}

inline String
to_time(const String& s) {
  return to_time(std::stod(s));
}

inline constexpr std::array<StringRef, 9> size_suffixes = {
    "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

String
to_byte(f64 bytes) {
  int index = 0;
  for (; bytes >= 1000.0; bytes /= 1024.0, ++index)
    ;
  return format("{:.2f}{}", bytes, size_suffixes.at(index));
}

// If string size is over specified number of characters and it can be clipped,
//  display an ellipsis (...).
inline String
clip_string(const String& s, const usize& n) {
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
Vec<String>
textwrap(const String& text, usize width = 70) {
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
#endif // POAC_UTIL_PRETTY_HPP_
