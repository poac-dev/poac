#ifndef POAC_CMD_INIT_HPP
#define POAC_CMD_INIT_HPP

// std
#include <iostream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <string>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/cmd/new.hpp>
#include <poac/core/validator.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/termcolor2/literals_extra.hpp>

namespace poac::cmd::init {
    struct Options: structopt::sub_command {
        /// Use a binary (application) template [default]
        std::optional<bool> bin = false;
        /// Use a library template
        std::optional<bool> lib = false;
    };

    [[nodiscard]] mitama::result<void, std::string>
    init(std::string_view package_name, init::Options&& opts) {
        spdlog::trace("Creating ./poac.toml");
        std::ofstream ofs_config("poac.toml");

        const bool is_bin = !opts.lib.value();
        if (is_bin) {
            ofs_config << _new::files::poac_toml(package_name);
        } else {
            ofs_config << _new::files::poac_toml(package_name);
        }

        using termcolor2::color_literals::operator""_bold_green;
        spdlog::info(
            "{:>25} {} `{}` package",
            "Created"_bold_green,
            is_bin ? "binary (application)" : "library",
            package_name
        );
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    exec(Options&& opts) {
        if (opts.bin.value() && opts.lib.value()) {
            return mitama::failure(
                "cannot specify both lib and binary outputs"
            );
        } else if (core::validator::required_config_exists().is_ok()) {
            return mitama::failure(
                "cannot run on existing poac packages"
            );
        }

        const std::string package_name = std::filesystem::current_path().stem().string();
        spdlog::trace(
            "Validating the package name `{}`", package_name
        );
        MITAMA_TRY(core::validator::valid_package_name(package_name));

        return init(package_name, std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_INIT_HPP
