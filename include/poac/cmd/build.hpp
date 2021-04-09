#ifndef POAC_CMD_BUILD_HPP
#define POAC_CMD_BUILD_HPP

// std
#include <filesystem>
#include <string>

// external
#include <mitama/result/result.hpp>
#include <toml.hpp>

// internal
#include <poac/core/builder.hpp>
#include <poac/core/resolver.hpp>
#include <poac/core/validator.hpp>

namespace poac::cmd::build {
    struct Options {
        core::builder::mode_t mode;
    };

    [[nodiscard]] mitama::result<std::filesystem::path, std::string>
    build(Options&& opts, const toml::value& config) {
        const auto resolved_deps = MITAMA_TRY(core::resolver::install_deps(config));
        const std::filesystem::path output_path = MITAMA_TRY(
            core::builder::build(config, opts.mode, resolved_deps)
        );
        return mitama::success(output_path);
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        MITAMA_TRY(core::validator::require_config_exists());
        const toml::value config = toml::parse("poac.toml");
        MITAMA_TRY(build(std::move(opts), config));
        return mitama::success();
    }
} // end namespace

#endif // !POAC_CMD_BUILD_HPP
