#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::clean {

struct Options : structopt::sub_command {
  /// Whether or not to clean release artifacts
  Option<bool> release = false;
  /// Clean artifacts of the specified profile
  Option<String> profile;
};

[[nodiscard]] auto exec(const Options& opts) -> Result<void>;

} // namespace poac::cmd::clean

STRUCTOPT(poac::cmd::clean::Options, release, profile);
