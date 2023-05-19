module;

// std
#include <filesystem>

// external
#include <structopt/app.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "../util/result-macros.hpp"

export module poac.cmd.clean;

// internal
import poac.config;
import poac.cmd.build;
import poac.data.manifest;
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;
import poac.util.validator;

namespace poac::cmd::clean {

export struct Options : structopt::sub_command {
  /// Whether or not to clean release artifacts
  Option<bool> release = false;
  /// Clean artifacts of the specified profile
  Option<String> profile;
};

[[nodiscard]] auto clean(const Options& opts)->Result<void> {
  const Option<String> profile =
      Try(util::validator::valid_profile(opts.profile, opts.release)
              .map_err(to_anyhow));
  if (profile.has_value() && profile.value() != "debug"
      && profile.value() != "release") {
    return Err<build::UnsupportedProfile>(profile.value());
  }

  const Path path =
      profile.has_value() ? config::out_dir / profile.value() : config::out_dir;

  spdlog::trace("Removing ./{}", path);

  if (fs::exists(path)) {
    log::status("Removing", fs::canonical(path).string());
    fs::remove_all(path);
  }

  return Ok();
}

export [[nodiscard]] auto exec(const Options& opts)->Result<void> {
  spdlog::trace("Checking if required config exists ...");
  Try(util::validator::required_config_exists().map_err(to_anyhow));

  return clean(opts);
}

} // namespace poac::cmd::clean

STRUCTOPT(poac::cmd::clean::Options, release, profile);
