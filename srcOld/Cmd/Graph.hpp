#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::graph {

struct Options : structopt::sub_command {
  // Perform only checks
  Option<Path> output_file = None;
};

[[nodiscard]] auto exec(const Options& opts) -> Result<void>;

} // namespace poac::cmd::graph

STRUCTOPT(poac::cmd::graph::Options, output_file);
