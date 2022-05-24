// Miscellaneous utility
#ifndef POAC_UTIL_MISC_HPP_
#define POAC_UTIL_MISC_HPP_

// internal
#include "poac/poac.hpp"

namespace poac::util::misc {

Vec<String>
split(const String& raw, const String& delim);

Option<String>
dupenv(const String& name);

// Inspired by https://stackoverflow.com/q/4891006
// Expand ~ to user home directory.
[[nodiscard]] Result<Path, String>
expand_user();

} // namespace poac::util::misc

#endif // POAC_UTIL_MISC_HPP_
