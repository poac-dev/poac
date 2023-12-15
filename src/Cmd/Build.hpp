#pragma once

// external
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include "../Core/Builder/Build.hpp"
#include "../Core/Resolver/Types.hpp" // ResolvedDeps
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::build {

using core::builder::build::Mode;
using core::resolver::ResolvedDeps;

struct Options : structopt::sub_command {
  /// Build artifacts in release mode, with optimizations
  Option<bool> release = false;
  /// Build artifacts with the specified profile
  Option<String> profile;
};

using FailedToBuild = Error<"failed to build package `{}`", String>;
using FailedToInstallDeps = Error<"failed to install dependencies">;
using UnsupportedProfile = Error<"unsupported profile `{}`", String>;

[[nodiscard]] auto
build(const Options& opts, const toml::value& manifest)
    -> Result<Option<Path>>;

[[nodiscard]] auto exec(const Options& opts) -> Result<void>;

} // namespace poac::cmd::build

STRUCTOPT(poac::cmd::build::Options, release, profile);
