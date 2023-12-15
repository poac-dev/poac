#include "Init.hpp"

// std
#include <iostream>
#include <string>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "./Create.hpp"
#include "../Data/Manifest.hpp"
#include "../Util/Format.hpp"
#include "../Util/Log.hpp"
#include "../Util/ResultMacros.hpp"
#include "../Util/Validator.hpp"
#include "../Config.hpp"

namespace poac::cmd::init {

using AlreadyInitialized = Error<"cannot initialize an existing poac package">;

[[nodiscard]] static auto init(const Options& opts, StringRef package_name)
    -> Result<void> {
  using create::ProjectType;

  spdlog::trace("Creating ./{}", data::manifest::NAME);
  std::ofstream ofs_config(data::manifest::NAME);

  const ProjectType type = create::opts_to_project_type(opts);
  ofs_config << create::files::poac_toml(package_name);

  log::status("Created", "{} `{}` package", to_string(type), package_name);
  return Ok();
}

[[nodiscard]] auto exec(const Options& opts) -> Result<void> {
  if (opts.bin.value() && opts.lib.value()) {
    return Err<create::PassingBothBinAndLib>();
  } else if (util::validator::required_config_exists(false).is_ok()) {
    return Err<AlreadyInitialized>();
  }

  const String package_name = config::cwd.stem().string();
  spdlog::trace("Validating the package name `{}`", package_name);
  Try(util::validator::valid_package_name(package_name).map_err(to_anyhow));

  return init(opts, package_name);
}

} // namespace poac::cmd::init
