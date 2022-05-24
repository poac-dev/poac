#ifndef POAC_UTIL_VERBOSITY_HPP_
#define POAC_UTIL_VERBOSITY_HPP_

// external
#include <spdlog/spdlog.h>

namespace poac::util::verbosity {

inline bool
is_verbose() {
  return spdlog::should_log(spdlog::level::trace);
}

inline bool
is_quiet() {
  return spdlog::level::off == spdlog::get_level();
}

} // namespace poac::util::verbosity

#endif // POAC_UTIL_VERBOSITY_HPP_
