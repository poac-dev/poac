// Miscellaneous utility
#ifndef POAC_UTIL_MISC_HPP_
#define POAC_UTIL_MISC_HPP_

// internal
#include "poac/poac.hpp"

namespace poac::util::misc {

Fn split(const String& raw, const String& delim)->Vec<String>;

Fn dupenv(const String& name)->Option<String>;

Fn getenv(const String& name, const String& default_v)->String;

// Inspired by https://stackoverflow.com/q/4891006
// Expand ~ to user home directory.
[[nodiscard]] Fn expand_user()->Result<Path, String>;

} // namespace poac::util::misc

#endif // POAC_UTIL_MISC_HPP_
