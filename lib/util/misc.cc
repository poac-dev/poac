// std
#include <cstdlib>

// external
#include <boost/algorithm/string.hpp>
#include <boost/predef.h> // NOLINT(build/include_order)

// internal
#include "poac/util/misc.hpp"

namespace poac::util::misc {

Vec<String>
split(const String& raw, const String& delim) {
  Vec<String> ret;
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
  boost::split(
      ret, raw, boost::is_any_of(delim), boost::algorithm::token_compress_on
  );
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

String
getenv(const String& name, const String& default_v) {
  if (const Option<String> env = dupenv(name)) {
    return env.value();
  } else {
    return default_v;
  }
}

// Inspired by https://stackoverflow.com/q/4891006
// Expand ~ to user home directory.
[[nodiscard]] Result<Path, String>
expand_user() {
  if (Option<String> home = dupenv("HOME")) {
    return Ok(home.value());
  } else if (Option<String> user = dupenv("USERPROFILE")) {
    return Ok(user.value());
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
