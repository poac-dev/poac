#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::init {

struct Options : structopt::sub_command {
  /// Use a binary (application) template [default]
  Option<bool> bin = false;
  /// Use a library template
  Option<bool> lib = false;
};

[[nodiscard]] auto exec(const Options& opts) -> Result<void>;

} // namespace poac::cmd::init

STRUCTOPT(poac::cmd::init::Options, bin, lib);
