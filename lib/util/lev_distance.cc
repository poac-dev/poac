// std
#include <utility> // std::pair

// internal
#include "poac/util/lev_distance.hpp"

namespace poac::util::lev_distance {

// ref: https://wandbox.org/permlink/zRjT41alOHdwcf00
usize
calc(StringRef a, StringRef b) {
  const usize asize = a.size();
  const usize bsize = b.size();

  // for all i and j, d[i,j] will hold the Levenshtein distance between the
  // first i characters of s and the first j characters of t
  Vec<Vec<usize>> d(asize + 1, Vec<usize>(bsize + 1));
  d[0][0] = 0;

  // source prefixes can be transformed into empty string by dropping all
  // characters
  for (usize i = 1; i <= asize; ++i) {
    d[i][0] = i;
  }

  // target prefixes can be reached from empty source prefix by inserting every
  // character
  for (usize j = 1; j <= bsize; ++j) {
    d[0][j] = j;
  }

  for (usize i = 1; i <= asize; ++i) {
    for (usize j = 1; j <= bsize; ++j) {
      const usize substCost = a[i - 1] == b[j - 1] ? 0 : 1;
      d[i][j] = std::min({
          d[i - 1][j] + 1, // deletion
          d[i][j - 1] + 1, // insertion
          d[i - 1][j - 1] + substCost // substitution
      });
    }
  }

  return d[asize][bsize];
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
find_similar_str(StringRef lhs, std::span<const StringRef> candidates) {
  // We need to check if `Candidates` has the exact case-insensitive string
  // because the Levenshtein distance match does not care about it.
  for (StringRef c : candidates) {
    if (equals_insensitive(lhs, c)) {
      return c;
    }
  }

  // Keep going with the Levenshtein distance match.
  // If the LHS size is less than 3, use the LHS size minus 1 and if not,
  // use the LHS size divided by 3.
  usize length = lhs.size();
  usize max_dist = length < 3 ? length - 1 : length / 3;

  Option<std::pair<StringRef, usize>> similar_str = None;
  for (StringRef c : candidates) {
    usize cur_dist = calc(lhs, c);
    if (cur_dist <= max_dist) {
      // The first similar string found || More similar string found
      if (!similar_str.has_value() || cur_dist < similar_str->second) {
        similar_str = {c, cur_dist};
      }
    }
  }

  if (similar_str.has_value()) {
    return similar_str->first;
  } else {
    return None;
  }
}

} // namespace poac::util::lev_distance
