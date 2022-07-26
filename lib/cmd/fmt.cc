// std
#include <span>

// external
#include <glob/glob.h> // NOLINT(build/include_order)
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <toml.hpp>

// internal
#include "poac/cmd/fmt.hpp"
#include "poac/data/manifest.hpp"
#include "poac/util/shell.hpp"
#include "poac/util/validator.hpp"
#include "poac/util/verbosity.hpp"

namespace poac::cmd::fmt {

using ClangFormatNotFound = Error<
    "`fmt` command requires `clang-format`; try installing it by:\n"
    "  apt/brew install clang-format">;

inline constexpr StringRef directories[] = {
    "examples", "include", "lib", "src", "tests"};
inline constexpr StringRef drogon_dirs[] = {"controllers", "filters", "views"};

inline constexpr StringRef extensions[] = {"c",   "c++", "cc",  "cpp",
                                           "cu",  "cuh", "cxx", "h",
                                           "h++", "hh",  "hpp", "hxx"};

inline constexpr StringRef patterns[] = {"./{}/*.{}", "./{}/**/*.{}"};

void
fmt_impl(std::span<const StringRef> dirs, Vec<Path>& targets) {
  for (StringRef d : dirs) {
    if (!fs::exists(d)) {
      spdlog::trace("Directory `{}` not found; skipping ...", d);
      continue;
    }

    for (StringRef e : extensions) {
      for (StringRef p : patterns) {
        const String search = format(::fmt::runtime(p), d, e);
        const Vec<Path> search_glob = glob::rglob(search);
        if (search_glob.empty()) {
          spdlog::trace("Glob `{}` not found; skipping ...", search);
          continue;
        }
        spdlog::trace("Glob `{}` found!", search);
        append(targets, search_glob);
      }
    }
  }
}

[[nodiscard]] Result<void>
fmt(const Options& opts, StringRef args) {
  Vec<Path> targets;

  fmt_impl(directories, targets);
  if (opts.drogon.value()) {
    fmt_impl(drogon_dirs, targets);
  }
  if (targets.empty()) {
    spdlog::info("no targets found.");
    return Ok();
  }

  const String clang_format =
      format("clang-format {} {}", args, ::fmt::join(targets, " "));
  spdlog::trace("Executing `{}`", clang_format);
  if (const i32 code = util::shell::Cmd(clang_format).exec_no_capture();
      code != 0) {
    spdlog::info("");
    return Err<SubprocessFailed>("clang-format", code);
  }
  return Ok();
}

[[nodiscard]] Result<void>
exec(const Options& opts) {
  spdlog::trace("Checking if `clang-format` command exists ...");
  if (!util::shell::has_command("clang-format")) {
    return Err<ClangFormatNotFound>();
  }

  spdlog::trace("Checking if required config exists ...");
  Try(util::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest = toml::parse(data::manifest::name);
  String name = toml::find<String>(manifest, "package", "name");
  if (name.empty()) {
    log::warn("project name is empty; try setting anything you want.");
    name = "<empty>";
  }

  String args = "--style=file --fallback-style=LLVM -Werror ";
  if (util::verbosity::is_verbose()) {
    args += "--verbose ";
  }
  if (opts.check.value()) {
    args += "--dry-run";
  } else {
    args += "-i";
    log::status("Formatting", name);
  }
  return fmt(opts, args);
}

} // namespace poac::cmd::fmt
