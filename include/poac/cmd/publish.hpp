#pragma once

// external
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::cmd::publish {

struct Options : structopt::sub_command {
  /// API Token obtained on poac.pm
  Option<String> token;

  /// Perform all checks without uploading
  Option<bool> dry_run = false;

  /// Allow dirty working directories to be packaged
  Option<bool> allow_dirty = false;
};

using NotImplemented = Error<
    "failed to publish; `publish` command is currently under development">;

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::publish

STRUCTOPT(poac::cmd::publish::Options, token, dry_run, allow_dirty);
