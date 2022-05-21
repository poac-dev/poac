#ifndef POAC_CMD_INIT_HPP
#define POAC_CMD_INIT_HPP

// std
#include <fstream>
#include <iostream>

// external
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/cmd/create.hpp>
#include <poac/config.hpp>
#include <poac/core/validator.hpp>
#include <poac/data/manifest.hpp>
#include <poac/poac.hpp>

namespace poac::cmd::init {

struct Options : structopt::sub_command {
  /// Use a binary (application) template [default]
  Option<bool> bin = false;
  /// Use a library template
  Option<bool> lib = false;
};

using AlreadyInitialized = Error<"cannot initialize an existing poac package">;

[[nodiscard]] Result<void>
init(const Options& opts, StringRef package_name) {
  using create::ProjectType;

  spdlog::trace("Creating ./{}", data::manifest::name);
  std::ofstream ofs_config(data::manifest::name);

  const ProjectType type = create::opts_to_project_type(opts);
  switch (type) {
    case ProjectType::Bin:
      ofs_config << create::files::poac_toml(package_name);
      break;
    case ProjectType::Lib:
      ofs_config << create::files::poac_toml(package_name);
      break;
    default:
      unreachable();
  }

  spdlog::info(
      "{:>25} {} `{}` package", "Created"_bold_green, to_string(type),
      package_name
  );
  return Ok();
}

[[nodiscard]] Result<void>
exec(const Options& opts) {
  if (opts.bin.value() && opts.lib.value()) {
    return Err<create::PassingBothBinAndLib>();
  } else if (core::validator::required_config_exists().is_ok()) {
    return Err<AlreadyInitialized>();
  }

  const String package_name = config::path::cur_dir.stem().string();
  spdlog::trace("Validating the package name `{}`", package_name);
  Try(core::validator::valid_package_name(package_name).map_err(to_anyhow));

  return init(opts, package_name);
}

} // namespace poac::cmd::init

STRUCTOPT(poac::cmd::init::Options, bin, lib);

#endif // !POAC_CMD_INIT_HPP
