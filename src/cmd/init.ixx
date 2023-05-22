module;

// std
#include <iostream>
#include <string>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <structopt/app.hpp>

// internal
#include "../util/result-macros.hpp"

export module poac.cmd.init;

import poac.cmd.create;
import poac.config;
import poac.data.manifest;
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;
import poac.util.validator;

namespace poac::cmd::init {

export struct Options : structopt::sub_command {
  /// Use a binary (application) template [default]
  Option<bool> bin = false;
  /// Use a library template
  Option<bool> lib = false;
};

using AlreadyInitialized = Error<"cannot initialize an existing poac package">;

[[nodiscard]] auto init(const Options& opts, StringRef package_name)
    -> Result<void> {
  using create::ProjectType;

  spdlog::trace("Creating ./{}", data::manifest::NAME);
  std::ofstream ofs_config(data::manifest::NAME);

  const ProjectType type = create::opts_to_project_type(opts);
  ofs_config << create::files::poac_toml(package_name);

  log::status("Created", "{} `{}` package", to_string(type), package_name);
  return Ok();
}

export [[nodiscard]] auto exec(const Options& opts) -> Result<void> {
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

STRUCTOPT(poac::cmd::init::Options, bin, lib);
