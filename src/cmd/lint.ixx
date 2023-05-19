module;

// std
#include <string>

// external
#include <boost/algorithm/string/join.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include "../util/result-macros.hpp"

export module poac.cmd.lint;

import poac.config;
import poac.cmd.fmt;
import poac.data.manifest;
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;
import poac.util.shell;
import poac.util.validator;
import poac.util.verbosity;

namespace poac::cmd::lint {

export struct Options : structopt::sub_command {
  // Dummy option: DO NOT USE
  Option<bool> dummy = false;
  // FIXME(ken-matsui): this is structopt limitation; I'd remove this.
};

inline constexpr StringRef CONFIG_NAME = "CPPLINT.cfg";

using CppLintNotFound = Error<
    "`lint` command requires `cpplint`; try:\n"
    "  pip install cpplint">;

[[nodiscard]] auto
lint(StringRef name, const Path& base_dir, Option<String> args)
    -> Result<void> {
  log::status("Linting", name);

  String cpplint = format("cd {} && cpplint ", base_dir.string());
  if (!args.has_value()) {
    spdlog::trace("Using cpplint config file: {} ...", base_dir / CONFIG_NAME);
  } else {
    spdlog::trace("Using pre-configured arguments ...");
    cpplint += format("{} ", args.value());
  }
  if (!util::verbosity::is_verbose()) {
    cpplint += "--quiet ";
  }
  cpplint += format("--exclude={} --recursive .", config::POAC_OUT);

  spdlog::trace("Executing `{}`", cpplint);
  if (const i32 code = util::shell::Cmd(cpplint).exec_no_capture(); code != 0) {
    spdlog::info("");
    return Err<SubprocessFailed>("cpplint", code);
  }
  return Ok();
}

export [[nodiscard]] auto exec([[maybe_unused]] const Options& opts)
    -> Result<void> {
  spdlog::trace("Checking if `cpplint` command exists ...");
  if (!util::shell::has_command("cpplint")) {
    return Err<CppLintNotFound>();
  }

  spdlog::trace("Checking if required config exists ...");
  const Path manifest_path =
      Try(util::validator::required_config_exists().map_err(to_anyhow));
  const Path base_dir = manifest_path.parent_path();

  spdlog::trace("Parsing the manifest file: {} ...", manifest_path);
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest =
      toml::parse(relative(manifest_path, config::cwd));
  const String name = toml::find<String>(manifest, "package", "name");
  const auto filters = toml::find_or<Vec<String>>(
      manifest, "lint", "cpplint", "filters", Vec<String>{}
  );
  if (!filters.empty()) {
    spdlog::trace("Using Poac manifest file for lint ...");
    String args = "--root=include ";
    for (const StringRef d : fmt::EXCLUDES) {
      args += format("--exclude={} ", d);
    }
    args += "--filter=" + boost::join(filters, ",");
    return lint(name, base_dir, args);
  }

  const Path config_path = base_dir / CONFIG_NAME;
  spdlog::trace("Checking if cpplint config exists: {} ...", config_path);
  if (fs::exists(config_path)) {
    spdlog::trace("Using cpplint config file: {} ...", config_path);
    return lint(name, base_dir, None);
  }

  String args;
  if (fs::exists(config::include_dir)) {
    args += "--root=include ";
  }
  if (2011 < toml::find<i64>(manifest, "package", "edition")) {
    args += "--filter=-build/c++11 ";
  }
  return lint(name, base_dir, args);
}

} // namespace poac::cmd::lint

STRUCTOPT(poac::cmd::lint::Options, dummy);
