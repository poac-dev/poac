#ifndef POAC_CMD_BUILD_HPP
#define POAC_CMD_BUILD_HPP

// std
#include <future>
#include <string>
#include <vector>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>

// internal
#include <poac/core/builder.hpp>
//#include <poac/io/config.hpp>

namespace poac::cmd::build {
    struct Options {
        core::builder::Mode mode;
    };

    [[nodiscard]] mitama::result<void, std::string>
    build(Options&& opts) {
//        std::future<std::optional<io::config::Config>>&& config,
        // if (const auto error = core::resolver::install_deps()) {
        //    return error;
        // }
//        core::Builder bs(config.get(), opts.mode, opts.verbose);
//        if (const auto error = bs.build()) {
//            return error;
//        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        return build(std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_BUILD_HPP
