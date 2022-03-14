#ifndef POAC_CORE_BUILDER_NINJA_LOG_HPP
#define POAC_CORE_BUILDER_NINJA_LOG_HPP

// std
#include <filesystem>
#include <string>

// external
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <ninja/load_status.h> // LoadStatus
#include <spdlog/spdlog.h>

// internal
#include <poac/core/builder/ninja/data.hpp>

namespace poac::core::builder::ninja::log {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using FailedToLoadBuildLog =
            error<"loading build log `{0}`: {1}", std::string, std::string>;

        using FailedToOpenBuildLog =
            error<"opening build log: {0}", std::string>;

        using FailedToLoadDepsLog =
            error<"loading deps log `{0}`: {1}", std::string, std::string>;

        using FailedToOpenDepsLog =
            error<"opening deps log: {0}", std::string>;
    };

    inline const std::string build_log_file_name = ".ninja_log";
    inline const std::string deps_log_file_name = ".ninja_deps";

    [[nodiscard]] anyhow::result<void>
    load_build_log(data::NinjaMain& ninja_main) {
        std::filesystem::path log_path = ninja_main.build_dir / build_log_file_name;

        std::string err;
        const LoadStatus status = ninja_main.build_log.Load(log_path, &err);
        if (status == LOAD_ERROR) {
            return anyhow::failure<Error::FailedToLoadBuildLog>(log_path.string(), err);
        }
        if (!err.empty()) {
            // Hack: Load() can return a warning via err by returning LOAD_SUCCESS.
            spdlog::warn(err);
            err.clear();
        }

        if (!ninja_main.build_log.OpenForWrite(log_path, ninja_main, &err)) {
            return anyhow::failure<Error::FailedToOpenBuildLog>(err);
        }
        return mitama::success();
    }

    [[nodiscard]] anyhow::result<void>
    load_deps_log(data::NinjaMain& ninja_main) {
        std::filesystem::path log_path = ninja_main.build_dir / deps_log_file_name;

        std::string err;
        const LoadStatus status = ninja_main.deps_log.Load(log_path, &ninja_main.state, &err);
        if (status == LOAD_ERROR) {
            return anyhow::failure<Error::FailedToLoadDepsLog>(log_path.string(), err);
        }
        if (!err.empty()) {
            // Hack: Load() can return a warning via err by returning LOAD_SUCCESS.
            spdlog::warn(err);
            err.clear();
        }

        if (!ninja_main.deps_log.OpenForWrite(log_path, &err)) {
            return anyhow::failure<Error::FailedToOpenDepsLog>(err);
        }
        return mitama::success();
    }
}

#endif // !POAC_CORE_BUILDER_NINJA_LOG_HPP
