#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "poac/poac.hpp"

namespace poac::cmd::run {

struct Options : structopt::sub_command {
  /// Build artifacts in release mode, with optimizations
  Option<bool> release = false;
  /// Build artifacts with the specified profile
  Option<String> profile;
};

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::run

STRUCTOPT(poac::cmd::run::Options, release, profile);
