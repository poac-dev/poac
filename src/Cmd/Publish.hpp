#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::publish {

struct Options : structopt::sub_command {
  /// API Token obtained on poac.dev
  Option<String> token;

  /// Perform all checks without uploading
  Option<bool> dry_run = false;

  /// Allow dirty working directories to be packaged
  Option<bool> allow_dirty = false;
};

[[nodiscard]] auto exec(const Options& opts) -> Result<void>;

} // namespace poac::cmd::publish

STRUCTOPT(poac::cmd::publish::Options, token, dry_run, allow_dirty);
