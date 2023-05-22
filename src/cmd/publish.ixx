module;

// std
#include <array>
#include <fstream>

// external
#include <structopt/app.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <toml.hpp>

// internal
#include "../util/result-macros.hpp"

export module poac.cmd.publish;

import poac.config;
import poac.cmd.build;
import poac.cmd.login;
import poac.data.manifest;
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;
import poac.util.validator;

namespace poac::cmd::publish {

export struct Options : structopt::sub_command {
  /// API Token obtained on poac.dev
  Option<String> token;

  /// Perform all checks without uploading
  Option<bool> dry_run = false;

  /// Allow dirty working directories to be packaged
  Option<bool> allow_dirty = false;
};

using APITokenNotFound = Error<
    "API token not found; please provide it through `--token "
    "$YOUR_TOKEN` or `poac login $YOUR_TOKEN`. If you do not have "
    "it yet, try signing up into poac.dev and generate a token at:\n"
    "  https://poac.dev/settings/tokens">;
using FailedToReadCred = Error<
    "failed to read credentials from {0}; do not edit it manually "
    "and use the `poac login` command.",
    Path>;
using FailedToReadManifest = Error<
    "failed to read a manifest file ({0}) for this package. Make "
    "sure to the current directory was set up using Poac.",
    Path>;

using NotImplemented = Error<
    "failed to publish; `publish` command is currently under development">;

[[nodiscard]] auto get_manifest()->Result<toml::value> {
  spdlog::trace("Checking if required config exists ...");
  Try(util::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  return Ok(toml::parse(data::manifest::NAME));
}

[[nodiscard]] auto get_token(const Options& opts)->Result<String> {
  if (opts.token.has_value()) {
    Try(login::check_token(opts.token.value()));
    return Ok(opts.token.value());
  } else {
    // Retrieve a token from `~/.poac/credentials`
    if (!fs::exists(config::cred_file)) {
      return Err<APITokenNotFound>();
    }

    const toml::value cred = toml::parse(config::cred_file);
    if (!cred.contains("registry")) {
      return Err<FailedToReadCred>(config::cred_file);
    }
    const String token = toml::find<String>(cred, "registry", "token");
    Try(login::check_token(token));
    return Ok(token);
  }
}

export [[nodiscard]] auto exec(const Options& opts)->Result<void> {
  const String token = Try(get_token(opts));
  const toml::value manifest = Try(get_manifest());
  const data::manifest::PartialPackage package =
      Try(util::validator::valid_manifest(manifest).map_err(to_anyhow));

  // if readme is specified, readme exists (and read it)

  // if (allow-dirty is false)
  // no changes from HEAD
  // no changes from tag (using clone?)

  // The `repository` key should be installable

  // Check if all dependencies are using Poac's registry
  // (to prevent unintentional errors)

  // Check buildablity
  Try(cmd::build::build(cmd::build::Options{.release = true}, manifest)
          .with_context([&manifest] {
            return Err<cmd::build::FailedToBuild>(
                       toml::find<String>(manifest, "package", "name")
            )
                .get();
          }));

  // download tar.gz and get hash
  // Run publish

  return Err<NotImplemented>();
}

} // namespace poac::cmd::publish

STRUCTOPT(poac::cmd::publish::Options, token, dry_run, allow_dirty);
