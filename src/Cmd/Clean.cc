#include "Clean.hpp"

// std
#include <filesystem>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "../Config.hpp"
#include "../Cmd/Build.hpp"
#include "../Data/Manifest.hpp"
#include "../Util/Format.hpp"
#include "../Util/Log.hpp"
#include "../Util/ResultMacros.hpp"
#include "../Util/Validator.hpp"

namespace poac::cmd::clean {

[[nodiscard]] static auto clean(const Options& opts) -> Result<void> {
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

[[nodiscard]] auto exec(const Options& opts) -> Result<void> {
  spdlog::trace("Checking if required config exists ...");
  Try(util::validator::required_config_exists().map_err(to_anyhow));

  return clean(opts);
}

}
