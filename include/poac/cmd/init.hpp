#ifndef POAC_CMD_INIT_HPP
#define POAC_CMD_INIT_HPP

// std
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

// external
#include <mitama/result/result.hpp>

// internal
#include <poac/cmd/new.hpp>
#include <poac/io/path.hpp>
#include <poac/io/term.hpp>
#include <poac/io/config.hpp>
#include <poac/core/name.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>

namespace poac::cmd::init {
    struct Options {
        _new::ProjectType type;
    };

    [[nodiscard]] mitama::result<void, std::string>
    init(init::Options&& opts) {
        using termcolor2::color_literals::operator""_green;

        const std::string package_name = io::path::current.stem().string();
        if (const auto error = core::name::validate_package_name(package_name)) {
            return mitama::failure(error->what());
        }

        std::ofstream ofs_config("poac.toml");
        switch (opts.type) {
            case _new::ProjectType::Bin:
                ofs_config << _new::files::poac_toml(package_name);
                break;
            case _new::ProjectType::Lib:
                ofs_config << _new::files::poac_toml(package_name);
                break;
        }
        fmt::print(
            "{}{} `{}` package\n",
            "Created: "_green,
            opts.type,
            package_name
        );
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        if (io::config::detail::validate_config()) {
            return mitama::failure(
                "`poac init` cannot run on existing poac packages"
            );
        }
        return init(std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_INIT_HPP
