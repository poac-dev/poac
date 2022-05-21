#ifndef POAC_CORE_BUILDER_NINJA_BUILD_HPP
#define POAC_CORE_BUILDER_NINJA_BUILD_HPP

// std
#include <cstdlib> // setenv
#include <ostream>
#include <stdexcept>

// external
#include <ninja/build.h>
#include <ninja/graph.h>
#include <ninja/manifest_parser.h>
#include <ninja/state.h>
#include <ninja/status.h>
#include <spdlog/spdlog.h>
#include <toml.hpp>

// internal
#include <poac/poac.hpp>
#include <poac/core/builder/ninja/data.hpp>
#include <poac/core/builder/ninja/log.hpp>
#include <poac/core/builder/ninja/manifest.hpp>
#include <poac/core/builder/ninja/status_printer.hpp>
#include <poac/core/resolver.hpp>
#include <poac/util/verbosity.hpp>
#include <poac/config.hpp>

namespace poac::core::builder::ninja::build {
    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using GeneralError =
            error<
                "internal build system has been stopped with an error:\n{}",
                String
            >;
    };

    enum class Mode {
        debug,
        release,
    };

    String
    to_string(Mode mode) {
        switch (mode) {
            case Mode::debug:
                return "debug";
            case Mode::release:
                return "release";
            default:
                unreachable();
        }
    }

    std::ostream&
    operator<<(std::ostream& os, Mode mode) {
        switch (mode) {
            case Mode::debug:
                return (os << "dev");
            case Mode::release:
                return (os << "release");
            default:
                unreachable();
        }
    }

    /// Build the targets listed on the command line.
    [[nodiscard]] Result<void>
    run(data::NinjaMain& ninja_main, Status& status) {
        String err;
        Vec<Node*> targets = ninja_main.state.DefaultNodes(&err);
        if (!err.empty()) {
            return Err<Error::GeneralError>(err);
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
        for (usize i = 0; i < targets.size(); ++i) {
            if (!builder.AddTarget(targets[i], &err)) {
                if (!err.empty()) {
                    return Err<Error::GeneralError>(err);
                }
                // Added a target that is already up-to-date; not really an error.
            }
        }
        // Make sure restat rules do not see stale timestamps.
        ninja_main.disk_interface.AllowStatCache(false);

        if (builder.AlreadyUpToDate()) {
            spdlog::trace("nothing to do.");
            return Ok();
        }
        if (!builder.Build(&err)) {
            return Err<Error::GeneralError>(err);
        }
        return Ok();
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

    inline const String progress_status_format =
        format("{:>25} %f/%t: ", "Compiling"_bold_green);

    // Limit number of rebuilds, to prevent infinite loops.
    inline constexpr i32 rebuildLimit = 100;

    [[nodiscard]] Result<fs::path>
    start(
        const toml::value& poac_manifest,
        const Mode& mode,
        const resolver::ResolvedDeps& resolved_deps
    ) {
        BuildConfig config;
        // setenv("NINJA_NOT_SMART_TERMINAL", "", true);
        // setenv("NINJA_STATUS", progress_status_format.c_str(), true);
        ninja::StatusPrinter status(config, progress_status_format);

        const fs::path build_dir = config::path::output_dir / to_string(mode);
        fs::create_directories(build_dir);
        tryi(manifest::create(build_dir, poac_manifest, resolved_deps));

        for (i32 cycle = 1; cycle <= rebuildLimit; ++cycle) {
            data::NinjaMain ninja_main(config, build_dir);
            ManifestParserOptions parser_opts;
            parser_opts.dupe_edge_action_ = kDupeEdgeActionError;
            ManifestParser parser(
                &ninja_main.state,
                &ninja_main.disk_interface,
                parser_opts
            );
            String err;
            if (!parser.Load((ninja_main.build_dir / manifest::manifest_file_name).string(), &err)) {
                return Err<Error::GeneralError>(err);
            }

            tryi(log::load_build_log(ninja_main));
            tryi(log::load_deps_log(ninja_main));

            // Attempt to rebuild the manifest before building anything else
            if (manifest::rebuild(ninja_main, status, err)) {
                // Start the build over with the new manifest.
                continue;
            } else if (!err.empty()) {
                return Err<Error::GeneralError>(err);
            }

            tryi(run(ninja_main, status));
            return Ok(config::path::output_dir / to_string(mode));
        }
        return Err<Error::GeneralError>(format(
            "internal manifest still dirty after {} tries, perhaps system time is not set",
            rebuildLimit
        ));
    }
}

#endif // !POAC_CORE_BUILDER_NINJA_BUILD_HPP
