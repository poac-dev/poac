#ifndef POAC_CMD_FMT_HPP_
#define POAC_CMD_FMT_HPP_

// std
#include <iostream>
#include <string>

// external
#include <glob/glob.h>     // NOLINT(build/include_order)
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

namespace poac::cmd::fmt {

struct Options : structopt::sub_command {
  // Perform only checks
  Option<bool> check = false;
};

using ClangFormatNotFound = Error<
    "`fmt` command requires `clang-format`; try installing it by:"
    "  apt/brew install clang-format">;
using ClangFormatError = Error<"`clang-format` finished with return code 1">;

inline const Vec<StringRef> directories = {
    "examples", "include", "src", "tests"};

inline const Vec<StringRef> extensions = {"c",   "c++", "cc",  "cpp",
                                          "cu",  "cuh", "cxx", "h",
                                          "h++", "hh",  "hpp", "hxx"};

inline const Vec<StringRef> patterns = {"./{}/*.{}", "./{}/**/*.{}"};

[[nodiscard]] Result<void>
fmt(StringRef args) {
  Vec<fs::path> targets{};
  for (StringRef d : directories) {
    if (!fs::exists(d)) {
      spdlog::trace("Directory `{}` not found; skipping ...", d);
      continue;
    }

    for (StringRef e : extensions) {
      for (StringRef p : patterns) {
        const String search = format(p, d, e);
        const Vec<fs::path> search_glob = glob::rglob(search);
        if (search_glob.empty()) {
          spdlog::trace("Glob `{}` not found; skipping ...", search);
          continue;
        }
        append(targets, search_glob);
      }
    }
  }

  const String clang_format =
      format("clang-format {} {}", args, ::fmt::join(targets, " "));
  spdlog::trace("Executing `{}`", clang_format);
  if (!util::shell::Cmd(clang_format).exec_no_capture()) {
    spdlog::info("");
    return Err<ClangFormatError>();
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
  Try(core::validator::required_config_exists().map_err(to_anyhow));

  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest = toml::parse(data::manifest::name);
  const String name = toml::find<String>(manifest, "package", "name");

  String args = "--style=file --fallback-style=LLVM -Werror ";
  if (util::verbosity::is_verbose()) {
    args += "--verbose ";
  }
  if (opts.check.value()) {
    args += "--dry-run";
  } else {
    args += "-i";
    spdlog::info("{:>25} {}", "Formatting"_bold_green, name);
  }
  return fmt(args);
}

} // namespace poac::cmd::fmt

STRUCTOPT(poac::cmd::fmt::Options, check);

#endif // POAC_CMD_FMT_HPP_
