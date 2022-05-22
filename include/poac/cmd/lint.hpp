#ifndef POAC_CMD_LINT_HPP_
#define POAC_CMD_LINT_HPP_

// std
#include <fstream>
#include <iostream>
#include <string>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include <poac/cmd/create.hpp>
#include <poac/config.hpp>
#include <poac/core/validator.hpp>
#include <poac/data/manifest.hpp>
#include <poac/poac.hpp>
#include <poac/util/shell.hpp>
#include <poac/util/verbosity.hpp>

namespace poac::cmd::lint {

struct Options : structopt::sub_command {
  // Dummy option: DO NOT USE
  Option<bool> dummy = false;
  // FIXME(ken-matsui): this is structopt limitation; I'd remove this.
};

inline const fs::path config_file(config::path::cur_dir / "CPPLINT.cfg");

using CppLintNotFound = Error<
    "`lint` command requires `cpplint`; try installing it by:"
    "  pip install cpplint">;
using CppLintError = Error<"`cpplint` finished with return code 1">;

[[nodiscard]] Result<void>
lint(StringRef name, Option<String> cpplint_args) {
  spdlog::info("{:>25} {}", "Linting"_bold_green, name);

  String cpplint = "cpplint ";
  if (!cpplint_args.has_value()) {
    spdlog::trace("Using cpplint config file ({}) ...", config_file);
  } else {
    spdlog::trace("Using pre-configured arguments ...");
    cpplint += format("{} ", cpplint_args.value());
  }
  if (!util::verbosity::is_verbose()) {
    cpplint += "--quiet ";
  }
  cpplint += "--exclude=poac_output/* --recursive .";

  spdlog::trace("Executing `{}`", cpplint);
  if (util::shell::Cmd(cpplint).exec_no_capture()) {
    return Ok();
  } else {
    spdlog::info("");
    return Err<CppLintError>();
  }
}

[[nodiscard]] Result<void>
exec([[maybe_unused]] const Options& opts) {
  spdlog::trace("Checking if `cpplint` command exists ...");
  if (!util::shell::has_command("cpplint")) {
    return Err<CppLintNotFound>();
  }

  spdlog::trace("Checking if required config exists ...");
  Try(core::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest = toml::parse(data::manifest::name);
  const String name = toml::find<String>(manifest, "package", "name");

  spdlog::trace("Checking if cpplint config ({}) exists ...", config_file);
  if (fs::exists(config_file)) {
    return lint(name, None);
  }

  String cpplint_args;
  if (fs::exists(config::path::include_dir)) {
    cpplint_args += "--root=include ";
  }
  if (2011 < toml::find<i64>(manifest, "package", "edition")) {
    cpplint_args += "--filter=-build/c++11 ";
  }
  return lint(name, cpplint_args);
}

} // namespace poac::cmd::lint

STRUCTOPT(poac::cmd::lint::Options, dummy);

#endif // POAC_CMD_LINT_HPP_
