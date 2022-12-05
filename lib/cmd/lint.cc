// std
#include <string>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <toml.hpp>

// internal
#include "poac/cmd/lint.hpp"
#include "poac/data/manifest.hpp"
#include "poac/util/shell.hpp"
#include "poac/util/validator.hpp"
#include "poac/util/verbosity.hpp"

namespace poac::cmd::lint {

inline constexpr StringRef config_name = "CPPLINT.cfg";

using CppLintNotFound = Error<
    "`lint` command requires `cpplint`; try installing it by:\n"
    "  pip install cpplint">;

[[nodiscard]] auto
lint(StringRef name, const Path& base_dir, Option<String> args)
    -> Result<void> {
  log::status("Linting", name);

  String cpplint = format("cd {} && cpplint ", base_dir.string());
  if (!args.has_value()) {
    spdlog::trace("Using cpplint config file: {} ...", base_dir / config_name);
  } else {
    spdlog::trace("Using pre-configured arguments ...");
    cpplint += format("{} ", args.value());
  }
  if (!util::verbosity::is_verbose()) {
    cpplint += "--quiet ";
  }
  cpplint += "--exclude=poac-out/* --recursive .";

  spdlog::trace("Executing `{}`", cpplint);
  if (const i32 code = util::shell::Cmd(cpplint).exec_no_capture(); code != 0) {
    spdlog::info("");
    return Err<SubprocessFailed>("cpplint", code);
  }
  return Ok();
}

[[nodiscard]] auto
exec([[maybe_unused]] const Options& opts) -> Result<void> {
  spdlog::trace("Checking if `cpplint` command exists ...");
  if (!util::shell::has_command("cpplint")) {
    return Err<CppLintNotFound>();
  }

  spdlog::trace("Checking if required config exists ...");
  const Path manifest_path =
      Try(util::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file: {} ...", manifest_path);
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest =
      toml::parse(relative(manifest_path, config::path::cwd));
  const String name = toml::find<String>(manifest, "package", "name");

  const Path base_dir = manifest_path.parent_path();
  const Path config_path = base_dir / config_name;
  spdlog::trace("Checking if cpplint config exists: {} ...", config_path);
  if (fs::exists(config_path)) {
    spdlog::trace("Using cpplint config file: {} ...", config_path);
    return lint(name, base_dir, None);
  }

  String args;
  if (fs::exists(config::path::include_dir)) {
    args += "--root=include ";
  }
  if (2011 < toml::find<i64>(manifest, "package", "edition")) {
    args += "--filter=-build/c++11 ";
  }
  return lint(name, base_dir, args);
}

} // namespace poac::cmd::lint
