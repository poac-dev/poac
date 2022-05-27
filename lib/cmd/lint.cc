// std
#include <string>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <toml.hpp>

// internal
#include "poac/cmd/lint.hpp"
#include "poac/core/validator.hpp"
#include "poac/data/manifest.hpp"
#include "poac/util/shell.hpp"
#include "poac/util/verbosity.hpp"

namespace poac::cmd::lint {

[[nodiscard]] Result<void>
lint(StringRef name, Option<String> args) {
  log::status("Linting", name);

  String cpplint = "cpplint ";
  if (!args.has_value()) {
    spdlog::trace("Using cpplint config file ({}) ...", config_file);
  } else {
    spdlog::trace("Using pre-configured arguments ...");
    cpplint += format("{} ", args.value());
  }
  if (!util::verbosity::is_verbose()) {
    cpplint += "--quiet ";
  }
  cpplint += "--exclude=poac_output/* --recursive .";

  spdlog::trace("Executing `{}`", cpplint);
  if (const i32 code = util::shell::Cmd(cpplint).exec_no_capture(); code != 0) {
    spdlog::info("");
    return Err<SubprocessFailed>("cpplint", code);
  }
  return Ok();
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

  String args;
  if (fs::exists(config::path::include_dir)) {
    args += "--root=include ";
  }
  if (2011 < toml::find<i64>(manifest, "package", "edition")) {
    args += "--filter=-build/c++11 ";
  }
  return lint(name, args);
}

} // namespace poac::cmd::lint
