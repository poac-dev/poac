#pragma once

// external
#include <spdlog/spdlog.h>

namespace poac::util::verbosity {

inline auto is_verbose() -> bool {
  return spdlog::should_log(spdlog::level::trace);
}

inline auto is_quiet() -> bool {
  return spdlog::level::off == spdlog::get_level();
}

} // namespace poac::util::verbosity
