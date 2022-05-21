// Miscellaneous utility
#ifndef POAC_UTIL_MISC_HPP
#define POAC_UTIL_MISC_HPP

// std
#include <cstdlib>
#include <iostream>

// external
#include <boost/algorithm/string.hpp>
#include <boost/predef.h>

// internal
#include <poac/poac.hpp>

namespace poac::util::misc {

Vec<String>
split(const String& raw, const String& delim) {
  using boost::is_any_of;
  using boost::algorithm::token_compress_on;

  Vec<String> ret;
  boost::split(ret, raw, is_any_of(delim), token_compress_on);
  return ret;
}

Option<String>
dupenv(const String& name) {
#if BOOST_COMP_MSVC
  char* env;
  usize len;
  if (_dupenv_s(&env, &len, name.c_str())) {
    return None;
  } else {
    String env_s(env);
    std::free(env);
    return env_s;
  }
#else
  if (const char* env = std::getenv(name.c_str())) {
    return env;
  } else {
    return None;
  }
#endif
}

// Inspired by https://stackoverflow.com/q/4891006
// Expand ~ to user home directory.
[[nodiscard]] Result<fs::path, String>
expand_user() {
  auto home = dupenv("HOME");
  if (home || (home = dupenv("USERPROFILE"))) {
    return Ok(home.value());
  } else {
    const auto home_drive = dupenv("HOMEDRIVE");
    const auto home_path = dupenv("HOMEPATH");
    if (home_drive && home_path) {
      return Ok(home_drive.value() + home_path.value());
    }
    return Err("could not get home directory");
  }
}

} // namespace poac::util::misc

#endif // POAC_UTIL_MISC_HPP
