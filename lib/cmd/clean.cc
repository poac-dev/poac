// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "poac/cmd/clean.hpp"
#include "poac/config.hpp"
#include "poac/data/manifest.hpp"
#include "poac/util/validator.hpp"

namespace poac::cmd::clean {

[[nodiscard]] Fn clean(const Options& opts)->Result<void> {
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

[[nodiscard]] Fn exec(const Options& opts)->Result<void> {
  spdlog::trace("Checking if required config exists ...");
  Try(util::validator::required_config_exists().map_err(to_anyhow));

  return clean(opts);
}

} // namespace poac::cmd::clean
