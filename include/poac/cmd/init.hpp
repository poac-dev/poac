#ifndef POAC_CMD_INIT_HPP
#define POAC_CMD_INIT_HPP

// Std
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <optional>

// Internal
#include <poac/cmd/new.hpp>
#include <poac/io/path.hpp>
#include <poac/io/term.hpp>
#include <poac/io/config.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/util/clap/clap.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>

namespace poac::cmd::init {
    struct Options {
        _new::ProjectType type;
    };

    [[nodiscard]] std::optional<core::except::Error>
    init(init::Options&& opts) {
        using termcolor2::color_literals::operator""_green;

        if (const auto error = core::name::validate_package_name(io::path::current.stem().string())) {
            return error;
        }

        std::cout << "Created: "_green;
        std::ofstream ofs_config("poac.toml");
        switch (opts.type) {
            case _new::ProjectType::Bin:
                ofs_config << _new::files::poac_toml(io::path::current.stem().string());
                break;
            case _new::ProjectType::Lib:
                ofs_config << _new::files::poac_toml(io::path::current.stem().string());
                break;
        }
        std::cout << opts.type << " package" << std::endl;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(Options&& opts) {
        if (io::config::detail::validate_config()) {
            return core::except::Error::General{
                "`poac init` cannot run on existing poac packages"
            };
        }
        return init(std::move(opts));
    }
} // end namespace

#endif // !POAC_CMD_INIT_HPP
