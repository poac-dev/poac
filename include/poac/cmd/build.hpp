#ifndef POAC_CMD_BUILD_HPP_
#define POAC_CMD_BUILD_HPP_

// external
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include "poac/config.hpp"
#include "poac/core/builder/build.hpp"
#include "poac/core/resolver/types.hpp" // ResolvedDeps
#include "poac/poac.hpp"

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

[[nodiscard]] Result<Path>
build_impl(
    const toml::value& manifest, const Mode& mode,
    const ResolvedDeps& resolved_deps
);

[[nodiscard]] Result<Option<Path>>
build(const Options& opts, const toml::value& manifest);

[[nodiscard]] Result<void>
exec(const Options& opts);

} // namespace poac::cmd::build

STRUCTOPT(poac::cmd::build::Options, release, profile);

#endif // POAC_CMD_BUILD_HPP_
