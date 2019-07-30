#ifndef POAC_OPTS_BUILD_HPP
#define POAC_OPTS_BUILD_HPP

#include <string>
#include <vector>
#include <optional>

#include <boost/filesystem.hpp>

#include <poac/core/builder.hpp>
#include <poac/core/except.hpp>
#include <poac/io/config.hpp>
#include <poac/util/argparse.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::build {
    constexpr auto summary = termcolor2::make_string("Compile a project and all sources that depend on its");
    constexpr auto options = termcolor2::make_string("[--release, -v | --verbose]");

    enum class Mode {
        Debug,
        Release,
    };

    struct Options {
        Mode mode;
        bool verbose;
    };

    [[nodiscard]] std::optional<core::except::Error>
    build(std::optional<io::config::Config>&&, build::Options&& opts) {
        // if (const auto error = core::resolver::install_deps()) {
        //    return error;
        // }
        core::Builder bs(opts.verbose);
        // if (const auto error = bs.build()) {
        //    return error;
        // }

        // TODO: install package to ~/.poac/src/
        // TODO: header-only -> -I~/.poac/src/${package}/include
        // TODO: build-required -> cp ~/.poac/src/${package} target/debug/deps/ -> build

        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::config::Config>&& config, std::vector<std::string>&& args) {
        if (args.size() > 1) {
            return core::except::Error::InvalidSecondArg::Build;
        }
        build::Options opts{};
        opts.mode = util::argparse::use(args, "--release") ? Mode::Release : Mode::Debug;
        opts.verbose = util::argparse::use(args, "-v", "--verbose");
        return build::build(std::move(config), std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_BUILD_HPP
