// external
#include <ninja/load_status.h> // LoadStatus // NOLINT(build/include_order)
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "poac/core/builder/log.hpp"

namespace poac::core::builder::log {

[[nodiscard]] Fn load_build_log(data::NinjaMain& ninja_main)->Result<void> {
  const Path log_path = ninja_main.build_dir / BUILD_LOG_FILE_NAME;

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

[[nodiscard]] Fn load_deps_log(data::NinjaMain& ninja_main)->Result<void> {
  const Path log_path = ninja_main.build_dir / DEPS_LOG_FILE_NAME;

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

} // namespace poac::core::builder::log
