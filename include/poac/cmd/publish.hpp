#ifndef POAC_CMD_PUBLISH_HPP_
#define POAC_CMD_PUBLISH_HPP_

// std
#include <array>
#include <fstream>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include <poac/cmd/build.hpp>
#include <poac/cmd/login.hpp>
#include <poac/config.hpp>
#include <poac/core/validator.hpp>
#include <poac/data/manifest.hpp>
#include <poac/poac.hpp>
#include <poac/util/net.hpp>

namespace poac::cmd::publish {

struct Options : structopt::sub_command {
  /// API Token obtained on poac.pm
  Option<String> token;

  /// Perform all checks without uploading
  Option<bool> dry_run = false;

  /// Allow dirty working directories to be packaged
  Option<bool> allow_dirty = false;
};

using APITokenNotFound = Error<
    "API token not found; please provide it through `--token "
    "$YOUR_TOKEN` or `poac login $YOUR_TOKEN`. If you do not have "
    "it yet, try signing up into poac.pm and generate a token at:\n"
    "  https://poac.pm/settings/tokens">;
using FailedToReadCred = Error<
    "failed to read credentials from {0}; do not edit it manually "
    "and use the `poac login` command.",
    fs::path>;
using FailedToReadManifest = Error<
    "failed to read a manifest file ({0}) for this package. Make "
    "sure to the current directory was set up using Poac.",
    fs::path>;

[[nodiscard]] anyhow::result<toml::value>
get_manifest() {
  spdlog::trace("Checking if required config exists ...");
  Try(core::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  return Ok(toml::parse(data::manifest::name));
}

[[nodiscard]] anyhow::result<String>
get_token(const Options& opts) {
  if (opts.token.has_value()) {
    Try(login::check_token(opts.token.value()));
    return Ok(opts.token.value());
  } else {
    // Retrieve a token from `~/.poac/credentials`
    if (!fs::exists(config::path::cred_file)) {
      return Err<APITokenNotFound>();
    }

    const toml::value cred = toml::parse(config::path::cred_file);
    if (!cred.contains("registry")) {
      return Err<FailedToReadCred>(config::path::cred_file);
    }
    const String token = toml::find<String>(cred, "registry", "token");
    Try(login::check_token(token));
    return Ok(token);
  }
}

[[nodiscard]] anyhow::result<void>
exec(const Options& opts) {
  const String token = Try(get_token(opts));
  const toml::value manifest = Try(get_manifest());
  const data::manifest::PartialPackage package =
      Try(core::validator::valid_manifest(manifest).map_err(to_anyhow));

  // if readme is specified, readme exists (and read)

  // if (allow-dirty is false)
  // no changes from HEAD
  // no changes from tag (using clone?)

  // Can be built

  // download tar.gz and get hash
  // Run publish

  // Check buildablity
  Try(cmd::build::build(cmd::build::Options{.release = true}, manifest)
          .with_context([&manifest] {
            return Err<cmd::build::FailedToBuild>(
                       toml::find<String>(manifest, "package", "name")
            )
                .get();
          }));

  return Ok();
}

} // namespace poac::cmd::publish

STRUCTOPT(poac::cmd::publish::Options, token, dry_run, allow_dirty);

#endif // POAC_CMD_PUBLISH_HPP_
