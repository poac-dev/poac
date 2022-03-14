#ifndef POAC_CORE_BUILDER_NINJA_MANIFEST_HPP
#define POAC_CORE_BUILDER_NINJA_MANIFEST_HPP

// std
#include <filesystem>
#include <fstream>
#include <string>

// external
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <ninja/build.h> // Builder
#include <ninja/graph.h> // Node

// internal
#include <poac/core/builder/ninja/syntax.hpp>
#include <poac/core/builder/ninja/data.hpp>
#include <poac/data/manifest.hpp>
#include <poac/config.hpp>

namespace poac::core::builder::ninja::manifest {
    namespace fs = std::filesystem;
    namespace anyhow = mitama::anyhow;

    inline const std::string manifest_file_name = "ninja.build";

    inline fs::file_time_type
    ninja_manifest_last_modified(const fs::path& build_dir) {
        return fs::last_write_time(build_dir / manifest_file_name);
    }

    inline bool
    is_outdated(const fs::path& build_dir) {
        if (!fs::exists(build_dir / manifest_file_name)) {
            return true;
        }
        using poac::data::manifest::poac_toml_last_modified;
        return ninja_manifest_last_modified(build_dir)
             < poac_toml_last_modified(config::path::current);
    }

    bool
    rebuild(data::NinjaMain& ninja_main, Status& status, std::string& err) {
        Node* node = ninja_main.state.LookupNode(
            (ninja_main.build_dir / manifest_file_name).string()
        );
        if (!node) {
            return false;
        }

        Builder builder(
            &ninja_main.state,
            ninja_main.config,
            &ninja_main.build_log,
            &ninja_main.deps_log,
            &ninja_main.disk_interface,
            &status,
            ninja_main.start_time_millis
        );
        if (!builder.AddTarget(node, &err)) {
            return false;
        }
        if (builder.AlreadyUpToDate()) {
            return false; // Not an error, but we didn't rebuild.
        }
        if (!builder.Build(&err)) {
            return false;
        }

        // The manifest was only rebuilt if it is now dirty (it may have been cleaned
        // by a restat).
        if (!node->dirty()) {
            // Reset the state to prevent problems like
            // https://github.com/ninja-build/ninja/issues/874
            ninja_main.state.Reset();
            return false;
        }
        return true;
    }

    std::string
    construct() {
        return "build system is not implemented yet";
    }

    [[nodiscard]] anyhow::result<void>
    create(const fs::path& build_dir) {
        std::ofstream ofs(build_dir / manifest_file_name, std::ios::out);

        const std::string content = construct();
        ofs << content;

        return mitama::success();
    }
}

#endif // !POAC_CORE_BUILDER_NINJA_MANIFEST_HPP
