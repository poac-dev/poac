#ifndef POAC_CMD_BUILD_HPP
#define POAC_CMD_BUILD_HPP

// std
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
        core::builder::Mode mode;
    };

    [[nodiscard]] mitama::result<void, std::string>
    build([[maybe_unused]] Options&& opts, const toml::value& config) {
        MITAMA_TRY(core::resolver::install_deps(config));
//        core::Builder bs(config.get(), opts.mode, opts.verbose);
//        MITAMA_TRY(bs.build());
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        MITAMA_TRY(core::validator::require_config_exists());
        const toml::value config = toml::parse("poac.toml");
        return build(std::move(opts), config);
    }
} // end namespace

#endif // !POAC_CMD_BUILD_HPP
