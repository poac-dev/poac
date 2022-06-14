#ifndef POAC_UTIL_LEV_DISTANCE_HPP_
#define POAC_UTIL_LEV_DISTANCE_HPP_

// std
#include <algorithm> // std::min, std::equal
#include <locale> // std::tolower
#include <span> // NOLINT(build/include_order)

// internal
#include "poac/poac.hpp"

namespace poac::util::lev_distance {

// ref: https://wandbox.org/permlink/zRjT41alOHdwcf00
usize
calc(StringRef a, StringRef b);

inline bool
equals_insensitive(StringRef a, StringRef b) {
  return std::equal(
      a.cbegin(), a.cend(), b.cbegin(), b.cend(),
      [](char a, char b) { return std::tolower(a) == std::tolower(b); }
  );
}

// ref: https://reviews.llvm.org/differential/changeset/?ref=3315514
/// Find a similar string in `candidates`.
///
/// \param lhs a string for a similar string in `Candidates`
///
/// \param candidates the candidates to find a similar string.
///
/// \returns a similar string if exists. If no similar string exists,
/// returns None.
Option<StringRef>
find_similar_str(StringRef lhs, std::span<const StringRef> candidates);

} // namespace poac::util::lev_distance

#endif // POAC_UTIL_LEV_DISTANCE_HPP_
