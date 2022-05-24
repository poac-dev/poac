#ifndef POAC_CORE_BUILDER_NINJA_LOG_HPP_
#define POAC_CORE_BUILDER_NINJA_LOG_HPP_

// std
#include <string>

// external
#include <ninja/load_status.h> // LoadStatus // NOLINT(build/include_order)
#include <spdlog/spdlog.h>     // NOLINT(build/include_order)

// internal
#include <poac/core/builder/ninja/data.hpp>
#include <poac/poac.hpp>

namespace poac::core::builder::ninja::log {

using FailedToLoadBuildLog =
    Error<"loading build log `{}`: {}", String, String>;
using FailedToOpenBuildLog = Error<"opening build log: {}", String>;
using FailedToLoadDepsLog = Error<"loading deps log `{}`: {}", String, String>;
using FailedToOpenDepsLog = Error<"opening deps log: {}", String>;

inline const String build_log_file_name = ".ninja_log";
inline const String deps_log_file_name = ".ninja_deps";

[[nodiscard]] Result<void>
load_build_log(data::NinjaMain& ninja_main) {
  Path log_path = ninja_main.build_dir / build_log_file_name;

  String err;
  const LoadStatus status = ninja_main.build_log.Load(log_path, &err);
  if (status == LOAD_ERROR) {
    return Err<FailedToLoadBuildLog>(log_path.string(), err);
  }
  if (!err.empty()) {
    // Hack: Load() can return a warning via err by returning LOAD_SUCCESS.
    spdlog::warn(err);
    err.clear();
  }

  if (!ninja_main.build_log.OpenForWrite(log_path, ninja_main, &err)) {
    return Err<FailedToOpenBuildLog>(err);
  }
  return Ok();
}

[[nodiscard]] Result<void>
load_deps_log(data::NinjaMain& ninja_main) {
  Path log_path = ninja_main.build_dir / deps_log_file_name;

  String err;
  const LoadStatus status =
      ninja_main.deps_log.Load(log_path, &ninja_main.state, &err);
  if (status == LOAD_ERROR) {
    return Err<FailedToLoadDepsLog>(log_path.string(), err);
  }
  if (!err.empty()) {
    // Hack: Load() can return a warning via err by returning LOAD_SUCCESS.
    spdlog::warn(err);
    err.clear();
  }

  if (!ninja_main.deps_log.OpenForWrite(log_path, &err)) {
    return Err<FailedToOpenDepsLog>(err);
  }
  return Ok();
}

} // namespace poac::core::builder::ninja::log

#endif // POAC_CORE_BUILDER_NINJA_LOG_HPP_
