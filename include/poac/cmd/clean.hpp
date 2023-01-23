#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "poac/cmd/build.hpp"
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::cmd::clean {

struct Options : structopt::sub_command {
  /// Whether or not to clean release artifacts
  Option<bool> release = false;
  /// Clean artifacts of the specified profile
  Option<String> profile;
};

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::clean

STRUCTOPT(poac::cmd::clean::Options, release, profile);
