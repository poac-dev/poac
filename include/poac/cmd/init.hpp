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
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/cmd/create.hpp>
#include <poac/core/validator.hpp>
#include <poac/data/manifest.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/termcolor2/literals_extra.hpp>

namespace poac::cmd::init {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    struct Options: structopt::sub_command {
        /// Use a binary (application) template [default]
        std::optional<bool> bin = false;
        /// Use a library template
        std::optional<bool> lib = false;
    };

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using AlreadyInitialized =
            error<"cannot initialize an existing poac package">;
    };

    [[nodiscard]] anyhow::result<void>
    init(const Options& opts, std::string_view package_name) {
        using create::ProjectType;

        spdlog::trace("Creating ./{}", data::manifest::manifest_file_name);
        std::ofstream ofs_config(data::manifest::manifest_file_name);

        const ProjectType type = create::opts_to_project_type(opts);
        switch (type) {
            case ProjectType::Bin:
                ofs_config << create::files::poac_toml(package_name);
                break;
            case ProjectType::Lib:
                ofs_config << create::files::poac_toml(package_name);
                break;
        }

        using termcolor2::color_literals::operator""_bold_green;
        spdlog::info(
            "{:>25} {} `{}` package",
            "Created"_bold_green,
            type,
            package_name
        );
        return mitama::success();
    }

    [[nodiscard]] anyhow::result<void>
    exec(const Options& opts) {
        if (opts.bin.value() && opts.lib.value()) {
            return anyhow::failure<create::Error::PassingBothBinAndLib>();
        } else if (core::validator::required_config_exists().is_ok()) {
            return anyhow::failure<Error::AlreadyInitialized>();
        }

        const std::string package_name = std::filesystem::current_path().stem().string();
        spdlog::trace("Validating the package name `{}`", package_name);
        MITAMA_TRY(
            core::validator::valid_package_name(package_name)
            .map_err([](const std::string& e){ return anyhow::anyhow(e); })
        );

        return init(opts, package_name);
    }
} // end namespace

STRUCTOPT(poac::cmd::init::Options, bin, lib);

#endif // !POAC_CMD_INIT_HPP
