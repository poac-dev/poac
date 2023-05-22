module;

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include "../util/result-macros.hpp"

export module poac.cmd.run;

import poac.cmd.build;
import poac.data.manifest;
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;
import poac.util.shell;
import poac.util.validator;

namespace poac::cmd::run {

export struct Options : structopt::sub_command {
  /// Build artifacts in release mode, with optimizations
  Option<bool> release = false;
  /// Build artifacts with the specified profile
  Option<String> profile;
};

export [[nodiscard]] auto exec(const Options& opts) -> Result<void> {
  spdlog::trace("Checking if required config exists ...");
  Try(util::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest = toml::parse(data::manifest::NAME);
  const String name = toml::find<String>(manifest, "package", "name");

  const Option<Path> output = Try(
      build::build({.release = opts.release, .profile = opts.profile}, manifest)
          .with_context([&name] {
            return Err<build::FailedToBuild>(name).get();
          })
  );
  if (!output.has_value()) {
    return Ok();
  }

  const Path executable = output.value() / name;
  log::status("Running", executable);
  if (const i32 code = util::shell::Cmd(executable).exec_no_capture();
      code != 0) {
    return Err<SubprocessFailed>(executable, code);
  }
  return Ok();
}

} // namespace poac::cmd::run

STRUCTOPT(poac::cmd::run::Options, release, profile);
