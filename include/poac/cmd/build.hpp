#ifndef POAC_CMD_BUILD_HPP
#define POAC_CMD_BUILD_HPP

// std
#include <string>
#include <vector>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>
#include <toml.hpp>

// internal
#include <poac/core/builder.hpp>
#include <poac/core/validator.hpp>
//#include <poac/io/config.hpp>

namespace poac::cmd::build {
    struct Options {
        core::builder::Mode mode;
    };

    [[nodiscard]] mitama::result<void, std::string>
    build(toml::value&& config, Options&& opts) {
//        MITAMA_TRY(core::resolver::install_deps(config);
//        core::Builder bs(config.get(), opts.mode, opts.verbose);
//        if (const auto error = bs.build()) {
//            return error;
//        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        MITAMA_TRY(core::validator::require_config_exists());
        toml::value&& config = toml::parse("poac.toml");
        return build(std::move(config), std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_BUILD_HPP
