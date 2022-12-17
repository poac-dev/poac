#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::cmd::fmt {

struct Options : structopt::sub_command {
  /// Perform only checks
  Option<bool> check = false;
};

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::fmt

STRUCTOPT(poac::cmd::fmt::Options, check);
