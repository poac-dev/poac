// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "poac/cmd/clean.hpp"
#include "poac/config.hpp"
#include "poac/data/manifest.hpp"
#include "poac/util/validator.hpp"

namespace poac::cmd::clean {

[[nodiscard]] Result<void>
clean(const Options& opts) {
  const Option<String> profile =
      Try(util::validator::valid_profile(opts.profile, opts.release)
              .map_err(to_anyhow));
  if (profile.has_value() && profile.value() != "debug"
      && profile.value() != "release") {
    return Err<build::UnsupportedProfile>(profile.value());
  }

  const Path path = profile.has_value()
                        ? config::path::output_dir / profile.value()
                        : config::path::output_dir;

  spdlog::trace("Removing ./{}", path);

  if (fs::exists(path)) {
    log::status("Removing", fs::canonical(path).string());
    fs::remove_all(path);
  }

  return Ok();
}

[[nodiscard]] Result<void>
exec(const Options& opts) {
  spdlog::trace("Checking if required config exists ...");
  Try(util::validator::required_config_exists().map_err(to_anyhow));

  return clean(opts);
}

} // namespace poac::cmd::clean
