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
#include <ninja/build_log.h>
#include <ninja/deps_log.h>
#include <ninja/disk_interface.h>
#include <ninja/graph.h>
#include <ninja/manifest_parser.h>
#include <ninja/metrics.h>
#include <ninja/state.h>
#include <ninja/status.h>
#include <spdlog/spdlog.h>

// internal
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
    run(State& state, const BuildConfig& config, Status& status, RealDiskInterface& disk_interface) {
        std::string err;
        std::vector<Node*> targets = state.DefaultNodes(&err);
        if (!err.empty()) {
            return anyhow::failure<Error::GeneralError>(err);
        }
        disk_interface.AllowStatCache(true);

        BuildLog build_log;
        DepsLog deps_log;
        Builder builder(
            &state,
            config,
            &build_log,
            &deps_log,
            &disk_interface,
            &status,
            GetTimeMillis()
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
        disk_interface.AllowStatCache(false);

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
    start(const toml::value&, const mode_t& mode, const resolver::resolved_deps_t&) {
        BuildConfig config;
        // Prevent setting `set_smart_terminal` as `true` in `StatusPrinter`
        config.verbosity = BuildConfig::VERBOSE;
        // Prevent being defined by users
        setenv("NINJA_STATUS", progress_status_format.c_str(), true);
        StatusPrinter status(config);

        // Loaded state (rules, nodes).
        State state;
        // Functions for accessing the disk.
        RealDiskInterface disk_interface;

        ManifestParser parser(&state, &disk_interface, ManifestParserOptions{});
        std::string err;
        // No Load() function call is needed because of no file IO,
        // so this has to use the ParserTest method instead of
        // the Parse method which is marked as private
        if (!parser.ParseTest(manifest::construct(), &err)) {
            return anyhow::failure<Error::GeneralError>(err);
        }
        MITAMA_TRY(run(state, config, status, disk_interface));
        return mitama::success(config::path::output_dir / to_string(mode));
    }
}

#endif // !POAC_CORE_BUILDER_NINJA_BUILD_HPP
