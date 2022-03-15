#ifndef POAC_CORE_BUILDER_NINJA_BUILD_HPP
#define POAC_CORE_BUILDER_NINJA_BUILD_HPP

// std
#include <cstdlib> // setenv
#include <cstdint>
#include <filesystem>
#include <ostream>
#include <stdexcept>
#include <string>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <ninja/build.h>
#include <ninja/graph.h>
#include <ninja/manifest_parser.h>
#include <ninja/state.h>
#include <ninja/status.h>
#include <spdlog/spdlog.h>
#include <toml.hpp>

// internal
#include <poac/core/builder/ninja/data.hpp>
#include <poac/core/builder/ninja/log.hpp>
#include <poac/core/builder/ninja/manifest.hpp>
#include <poac/core/resolver.hpp>
#include <poac/util/verbosity.hpp>
#include <poac/config.hpp>

namespace poac::core::builder::ninja::build {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using GeneralError =
            error<
                "internal build system has been stopped with an error:\n{0}",
                std::string
            >;
    };

    enum class mode_t {
        debug,
        release,
    };

    std::string
    to_string(mode_t mode) {
        switch (mode) {
            case mode_t::debug:
                return "debug";
            case mode_t::release:
                return "release";
            default:
                throw std::logic_error(
                    "To access out of range of the "
                    "enumeration values is undefined behavior."
                );
        }
    }

    std::ostream&
    operator<<(std::ostream& os, mode_t mode) {
        switch (mode) {
            case mode_t::debug:
                return (os << "dev");
            case mode_t::release:
                return (os << "release");
            default:
                throw std::logic_error(
                    "To access out of range of the "
                    "enumeration values is undefined behavior."
                );
        }
    }

    /// Build the targets listed on the command line.
    [[nodiscard]] anyhow::result<void>
    run(data::NinjaMain& ninja_main, Status& status) {
        std::string err;
        std::vector<Node*> targets = ninja_main.state.DefaultNodes(&err);
        if (!err.empty()) {
            return anyhow::failure<Error::GeneralError>(err);
        }
        ninja_main.disk_interface.AllowStatCache(true);

        Builder builder(
            &ninja_main.state,
            ninja_main.config,
            &ninja_main.build_log,
            &ninja_main.deps_log,
            &ninja_main.disk_interface,
            &status,
            ninja_main.start_time_millis
        );
        for (std::size_t i = 0; i < targets.size(); ++i) {
            if (!builder.AddTarget(targets[i], &err)) {
                if (!err.empty()) {
                    return anyhow::failure<Error::GeneralError>(err);
                }
                // Added a target that is already up-to-date; not really an error.
            }
        }
        // Make sure restat rules do not see stale timestamps.
        ninja_main.disk_interface.AllowStatCache(false);

        if (builder.AlreadyUpToDate()) {
            spdlog::trace("nothing to do.");
            return mitama::success();
        }
        if (!builder.Build(&err)) {
            return anyhow::failure<Error::GeneralError>(err);
        }
        return mitama::success();
    }

    inline BuildConfig::Verbosity
    get_ninja_verbosity() {
        if (util::verbosity::is_verbose()) {
            return BuildConfig::VERBOSE;
        } else if (util::verbosity::is_quiet()) {
            return BuildConfig::QUIET;
        } else {
            return BuildConfig::NORMAL;
        }
    }

    using termcolor2::color_literals::operator""_bold_green;
    inline const std::string progress_status_format =
        fmt::format("{:>25} %f/%t: ", "Compiling"_bold_green);

    // Limit number of rebuilds, to prevent infinite loops.
    inline constexpr int rebuildLimit = 100;

    [[nodiscard]] anyhow::result<std::filesystem::path>
    start(
        const toml::value& poac_manifest,
        const mode_t& mode,
        const resolver::resolved_deps_t& resolved_deps
    ) {
        BuildConfig config;
        // Prevent setting `set_smart_terminal` as `true` in `StatusPrinter`
        config.verbosity = BuildConfig::VERBOSE;
        // Prevent being defined by users
        setenv("NINJA_STATUS", progress_status_format.c_str(), true);
        StatusPrinter status(config);

        const std::filesystem::path build_dir = config::path::output_dir / to_string(mode);
        std::filesystem::create_directories(build_dir);
        MITAMA_TRY(manifest::create(build_dir, poac_manifest, resolved_deps));

        for (int cycle = 1; cycle <= rebuildLimit; ++cycle) {
            data::NinjaMain ninja_main(config, build_dir);
            ManifestParserOptions parser_opts;
            parser_opts.dupe_edge_action_ = kDupeEdgeActionError;
            ManifestParser parser(
                &ninja_main.state,
                &ninja_main.disk_interface,
                parser_opts
            );
            std::string err;
            if (!parser.Load((ninja_main.build_dir / manifest::manifest_file_name).string(), &err)) {
                return anyhow::failure<Error::GeneralError>(err);
            }

            MITAMA_TRY(log::load_build_log(ninja_main));
            MITAMA_TRY(log::load_deps_log(ninja_main));

            // Attempt to rebuild the manifest before building anything else
            if (manifest::rebuild(ninja_main, status, err)) {
                // Start the build over with the new manifest.
                continue;
            } else if (!err.empty()) {
                return anyhow::failure<Error::GeneralError>(err);
            }

            MITAMA_TRY(run(ninja_main, status));
            return mitama::success(config::path::output_dir / to_string(mode));
        }
        return anyhow::failure<Error::GeneralError>(fmt::format(
            "internal manifest still dirty after {} tries, perhaps system time is not set",
            rebuildLimit
        ));
    }
}

#endif // !POAC_CORE_BUILDER_NINJA_BUILD_HPP
