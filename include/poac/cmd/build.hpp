#ifndef POAC_CMD_BUILD_HPP
#define POAC_CMD_BUILD_HPP

// std
#include <filesystem>
#include <string>
#include <optional>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include <poac/core/builder/ninja/build.hpp>
#include <poac/core/resolver.hpp>
#include <poac/core/validator.hpp>
#include <poac/data/manifest.hpp>
#include <poac/util/pretty.hpp>

namespace poac::cmd::build {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    using core::builder::ninja::build::mode_t;
    using core::resolver::resolved_deps_t;

    struct Options: structopt::sub_command {
        /// Build artifacts in release mode, with optimizations
        std::optional<bool> release = false;
    };

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using FailedToBuild =
            error<"failed to build package `{0}`", std::string>;

        using FailedToInstallDeps =
            error<"failed to install dependencies">;
    };

    [[nodiscard]] anyhow::result<std::filesystem::path>
    build_impl(const toml::value& config, const mode_t& mode, const resolved_deps_t& resolved_deps) {
        spdlog::stopwatch sw;
        const std::filesystem::path output_path = MITAMA_TRY(
            core::builder::ninja::build::start(config, mode, resolved_deps)
        );

        using termcolor2::color_literals::operator""_bold_green;
        spdlog::info(
            "{:>25} {} target(s) in {}",
            "Finished"_bold_green,
            mode,
            util::pretty::to_time(sw.elapsed().count())
        );
        return mitama::success(output_path);
    }

    [[nodiscard]] anyhow::result<std::filesystem::path>
    build(const Options& opts, const toml::value& config) {
        spdlog::trace("Resolving dependencies ...");
        const auto resolved_deps = MITAMA_TRY(
            core::resolver::install_deps(config)
                .with_context([]{
                    return anyhow::failure<Error::FailedToInstallDeps>().get();
                })
        );

        const mode_t mode = opts.release.value() ? mode_t::release : mode_t::debug;
        const std::filesystem::path output_path = MITAMA_TRY(
            build_impl(config, mode, resolved_deps)
        );
        return mitama::success(output_path);
    }

    [[nodiscard]] anyhow::result<void>
    exec(const Options& opts) {
        spdlog::trace("Checking required config exists ...");
        MITAMA_TRY(
            core::validator::required_config_exists()
                .map_err([](const std::string& e){ return anyhow::anyhow(e); })
        );

        spdlog::trace("Parsing the manifest file ...");
        // TODO: parse as a static type, not toml::value
        const toml::value config = toml::parse(data::manifest::manifest_file_name);

        MITAMA_TRY(
            build(opts, config).with_context([&config]{
                return anyhow::failure<Error::FailedToBuild>(
                    toml::find<std::string>(config, "package", "name")
                ).get();
            })
        );
        return mitama::success();
    }
} // end namespace

STRUCTOPT(poac::cmd::build::Options, release);

#endif // !POAC_CMD_BUILD_HPP
