#ifndef POAC_UTIL_PRETTY_HPP_
#define POAC_UTIL_PRETTY_HPP_

// std
#include <array>
#include <chrono>

// internal
#include "poac/poac.hpp"

namespace poac::util::pretty {

Fn to_time(const f64& total_seconds)->String;

inline Fn to_time(const std::chrono::seconds& s)->String {
  return to_time(s.count());
}

inline Fn to_time(const String& s)->String { return to_time(std::stod(s)); }

inline constexpr std::array<StringRef, 9> size_suffixes = {
    "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

Fn to_byte(f64 bytes)->String;

// If string size is over specified number of characters and it can be clipped,
//  display an ellipsis (...).
inline Fn clip_string(const String& s, const usize& n)->String {
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
Fn textwrap(const String& text, usize width = 70)->Vec<String>;

} // namespace poac::util::pretty

#endif // POAC_UTIL_PRETTY_HPP_
