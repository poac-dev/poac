// std
#include <string>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <spdlog/stopwatch.h> // NOLINT(build/include_order)

// internal
#include "poac/cmd/build.hpp"
#include "poac/core/resolver.hpp" // install_deps
#include "poac/data/manifest.hpp"
#include "poac/util/log.hpp"
#include "poac/util/pretty.hpp" // to_time
#include "poac/util/validator.hpp"

namespace poac::cmd::build {

[[nodiscard]] Fn build_impl(
    const toml::value& manifest, const Mode& mode,
    const ResolvedDeps& resolved_deps
)
    ->Result<Path> {
  const spdlog::stopwatch sw;
  const Path output_path =
      Try(core::builder::build::start(manifest, mode, resolved_deps));

  log::status(
      "Finished", "{} target(s) in {}", to_string(mode),
      util::pretty::to_time(sw.elapsed().count())
  );
  return Ok(output_path);
}

[[nodiscard]] Fn build(const Options& opts, const toml::value& manifest)
    ->Result<Option<Path>> {
  const auto resolved_deps =
      Try(core::resolver::install_deps(manifest).with_context([] {
        return Err<FailedToInstallDeps>().get();
      }));

  // TODO(ken-matsui): We have to keep in mind a case of only dependencies
  //  require to be built, but this package does not.
  if (!fs::exists(config::main_cpp_file)) {
    log::status("Finished", "no build target(s) found");
    return Ok(None);
  }

  const auto profile =
      Try(util::validator::valid_profile(opts.profile, opts.release)
              .map_err(to_anyhow))
          .value_or("debug");
  if (profile != "debug" && profile != "release") {
    return Err<UnsupportedProfile>(profile);
  }

  const Mode mode = profile == "release" ? Mode::release : Mode::debug;
  const Path output_path = Try(build_impl(manifest, mode, resolved_deps));
  return Ok(output_path);
}

[[nodiscard]] Fn exec(const Options& opts)->Result<void> {
  spdlog::trace("Checking if required config exists ...");
  Try(util::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest = toml::parse(data::manifest::NAME);

  Try(build(opts, manifest).with_context([&manifest] {
    return Err<FailedToBuild>(toml::find<String>(manifest, "package", "name"))
        .get();
  }));
  return Ok();
}

} // namespace poac::cmd::build
