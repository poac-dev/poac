module;

// std
#include <string>

// external
#include <ninja/load_status.h> // LoadStatus // NOLINT(build/include_order)
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

export module poac.core.builder.log;

// internal
import poac.core.builder.data;
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;

namespace poac::core::builder::log {

using FailedToLoadBuildLog =
    Error<"loading build log `{}`: {}", String, String>;
using FailedToOpenBuildLog = Error<"opening build log: {}", String>;
using FailedToLoadDepsLog = Error<"loading deps log `{}`: {}", String, String>;
using FailedToOpenDepsLog = Error<"opening deps log: {}", String>;

inline constexpr StringRef BUILD_LOG_FILE_NAME = ".ninja_log";
inline constexpr StringRef DEPS_LOG_FILE_NAME = ".ninja_deps";

export [[nodiscard]] auto load_build_log(data::NinjaMain& ninja_main) -> Result<void> {
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

export [[nodiscard]] auto load_deps_log(data::NinjaMain& ninja_main) -> Result<void> {
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
