#ifndef POAC_CMD_PUBLISH_HPP
#define POAC_CMD_PUBLISH_HPP

// std
#include <array>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include <poac/cmd/build.hpp>
#include <poac/cmd/login.hpp>
#include <poac/core/validator.hpp>
#include <poac/data/manifest.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/net.hpp>
#include <poac/config.hpp>

namespace poac::cmd::publish {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    struct Options: structopt::sub_command {
        /// API Token obtained on poac.pm
        std::optional<std::string> token;

        /// Perform all checks without uploading
        std::optional<bool> dry_run = false;

        /// Allow dirty working directories to be packaged
        std::optional<bool> allow_dirty = false;
    };

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using APITokenNotFound =
            error<
                "API token not found; please provide it through `--token "
                "$YOUR_TOKEN` or `poac login $YOUR_TOKEN`. If you do not have "
                "it yet, try signing up into poac.pm and generate a token at:\n"
                "  https://poac.pm/settings/tokens"
            >;

        using FailedToReadCred =
            error<
                "failed to read credentials from {0}; do not edit it manually "
                "and use the `poac login` command.",
                std::filesystem::path
            >;

        using FailedToReadManifest =
            error<
                "failed to read a manifest file ({0}) for this package. Make "
                "sure to the current directory was set up using Poac.",
                std::filesystem::path
            >;
    };

    [[nodiscard]] anyhow::result<toml::value>
    get_manifest() {
        spdlog::trace("Checking if required config exists ...");
        MITAMA_TRY(
            core::validator::required_config_exists()
                .map_err([](const std::string& e){ return anyhow::anyhow(e); })
        );

        spdlog::trace("Parsing the manifest file ...");
        // TODO: parse as a static type rather than toml::value
        return mitama::success(toml::parse(data::manifest::manifest_file_name));
    }

    [[nodiscard]] anyhow::result<std::string>
    get_token(const Options& opts) {
        if (opts.token.has_value()) {
            MITAMA_TRY(login::check_token(opts.token.value()));
            return mitama::success(opts.token.value());
        } else {
            // Retrieve a token from `~/.poac/credentials`
            if (!std::filesystem::exists(config::path::credentials)) {
                return anyhow::failure<Error::APITokenNotFound>();
            }

            const toml::value cred = toml::parse(config::path::credentials);
            if (!cred.contains("registry")) {
                return anyhow::failure<Error::FailedToReadCred>(config::path::credentials);
            }
            const std::string token = toml::find<std::string>(cred, "registry", "token");
            MITAMA_TRY(login::check_token(token));
            return mitama::success(token);
        }
    }

    [[nodiscard]] anyhow::result<void>
    exec(const Options& opts) {
        const std::string token = MITAMA_TRY(get_token(opts));
        const toml::value manifest = MITAMA_TRY(get_manifest());
        const data::manifest::PartialPackage package = MITAMA_TRY(
            core::validator::valid_manifest(manifest)
                .map_err([](const std::string& e){ return anyhow::anyhow(e); })
        );

        // if readme is specified, readme exists (and read)

        // if (allow-dirty is false)
        // no changes from HEAD
        // no changes from tag (using clone?)

        // Can be built

        // download tar.gz and get hash
        // Run publish

        // Check buildablity
        MITAMA_TRY(
            cmd::build::build(
                cmd::build::Options{.release = true},
                manifest
            ).with_context([&manifest]{
                return anyhow::failure<cmd::build::Error::FailedToBuild>(
                           toml::find<std::string>(manifest, "package", "name")
                ).get();
            })
        );

        return mitama::success();
    }
} // end namespace

STRUCTOPT(poac::cmd::publish::Options, token, dry_run, allow_dirty);

#endif // !POAC_CMD_PUBLISH_HPP
