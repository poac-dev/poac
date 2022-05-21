#ifndef POAC_CORE_BUILDER_NINJA_LOG_HPP
#define POAC_CORE_BUILDER_NINJA_LOG_HPP

// external
#include <ninja/load_status.h> // LoadStatus
#include <spdlog/spdlog.h>

// internal
#include <poac/poac.hpp>
#include <poac/core/builder/ninja/data.hpp>

namespace poac::core::builder::ninja::log {
    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using FailedToLoadBuildLog =
            error<"loading build log `{0}`: {1}", String, String>;

        using FailedToOpenBuildLog =
            error<"opening build log: {0}", String>;

        using FailedToLoadDepsLog =
            error<"loading deps log `{0}`: {1}", String, String>;

        using FailedToOpenDepsLog =
            error<"opening deps log: {0}", String>;
    };

    inline const String build_log_file_name = ".ninja_log";
    inline const String deps_log_file_name = ".ninja_deps";

    [[nodiscard]] Result<void>
    load_build_log(data::NinjaMain& ninja_main) {
        fs::path log_path = ninja_main.build_dir / build_log_file_name;

        String err;
        const LoadStatus status = ninja_main.build_log.Load(log_path, &err);
        if (status == LOAD_ERROR) {
            return Err<Error::FailedToLoadBuildLog>(log_path.string(), err);
        }
        if (!err.empty()) {
            // Hack: Load() can return a warning via err by returning LOAD_SUCCESS.
            spdlog::warn(err);
            err.clear();
        }

        if (!ninja_main.build_log.OpenForWrite(log_path, ninja_main, &err)) {
            return Err<Error::FailedToOpenBuildLog>(err);
        }
        return Ok();
    }

    [[nodiscard]] Result<void>
    load_deps_log(data::NinjaMain& ninja_main) {
        fs::path log_path = ninja_main.build_dir / deps_log_file_name;

        String err;
        const LoadStatus status = ninja_main.deps_log.Load(log_path, &ninja_main.state, &err);
        if (status == LOAD_ERROR) {
            return Err<Error::FailedToLoadDepsLog>(log_path.string(), err);
        }
        if (!err.empty()) {
            // Hack: Load() can return a warning via err by returning LOAD_SUCCESS.
            spdlog::warn(err);
            err.clear();
        }

        if (!ninja_main.deps_log.OpenForWrite(log_path, &err)) {
            return Err<Error::FailedToOpenDepsLog>(err);
        }
        return Ok();
    }
}

#endif // !POAC_CORE_BUILDER_NINJA_LOG_HPP
