#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::create {

struct Options : structopt::sub_command {
  /// Package name to create a new poac package
  String package_name;

  /// Use a binary (application) template [default]
  Option<bool> bin = false;
  /// Use a library template
  Option<bool> lib = false;
};

using PassingBothBinAndLib =
    Error<"cannot specify both lib and binary outputs">;

enum class ProjectType {
  Bin,
  Lib,
};

auto to_string(ProjectType kind) -> String;

namespace files {
  auto poac_toml(StringRef project_name) -> String;
} // namespace files

[[nodiscard]] auto exec(const Options& opts) -> Result<void>;

} // namespace poac::cmd::create

STRUCTOPT(poac::cmd::create::Options, package_name, bin, lib);
