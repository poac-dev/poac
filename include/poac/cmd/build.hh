#ifndef POAC_CMD_BUILD_HPP_
#define POAC_CMD_BUILD_HPP_

// std
#include <string>

// external
#include <spdlog/spdlog.h>    // NOLINT(build/include_order)
#include <spdlog/stopwatch.h> // NOLINT(build/include_order)
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include "poac/poac.hh"

#include <poac/config.hh>
#include <poac/core/builder/ninja/build.hh>
#include <poac/core/resolver.hh>
#include <poac/core/validator.hh>
#include <poac/data/manifest.hh>
#include <poac/util/pretty.hh>

namespace poac::cmd::build {

using core::builder::ninja::build::Mode;
using core::resolver::ResolvedDeps;

struct Options : structopt::sub_command {
  /// Build artifacts in release mode, with optimizations
  Option<bool> release = false;
};

using FailedToBuild = Error<"failed to build package `{}`", String>;
using FailedToInstallDeps = Error<"failed to install dependencies">;

[[nodiscard]] Result<Path>
build_impl(
    const toml::value& manifest, const Mode& mode,
    const ResolvedDeps& resolved_deps
) {
  spdlog::stopwatch sw;
  const Path output_path =
      Try(core::builder::ninja::build::start(manifest, mode, resolved_deps));

  log::status(
      "Finished"_bold_green, "{} target(s) in {}", to_string(mode),
      util::pretty::to_time(sw.elapsed().count())
  );
  return Ok(output_path);
}

[[nodiscard]] Result<Option<Path>>
build(const Options& opts, const toml::value& manifest) {
  const auto resolved_deps =
      Try(core::resolver::install_deps(manifest).with_context([] {
        return Err<FailedToInstallDeps>().get();
      }));

  // TODO(ken-matsui): We have to keep in mind a case of only dependencies
  //  require to be built, but this package does not.
  if (!fs::exists(config::path::main_cpp_file)) {
    log::status("Finished"_bold_green, "no build target(s) found");
    return Ok(None);
  }

  const Mode mode = opts.release.value() ? Mode::release : Mode::debug;
  const Path output_path = Try(build_impl(manifest, mode, resolved_deps));
  return Ok(output_path);
}

[[nodiscard]] Result<void>
exec(const Options& opts) {
  spdlog::trace("Checking if required config exists ...");
  Try(core::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest = toml::parse(data::manifest::name);

  Try(build(opts, manifest).with_context([&manifest] {
    return Err<FailedToBuild>(toml::find<String>(manifest, "package", "name"))
        .get();
  }));
  return Ok();
}

} // namespace poac::cmd::build

STRUCTOPT(poac::cmd::build::Options, release);

#endif // POAC_CMD_BUILD_HPP_
