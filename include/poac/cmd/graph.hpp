#pragma once

// std
#include <utility>

// external
#include <boost/graph/adjacency_list.hpp>
#include <structopt/app.hpp>

// internal
#include "poac/core/resolver.hpp"
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::cmd::graph {

struct Options : structopt::sub_command {
  // Perform only checks
  Option<Path> output_file = None;
};

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::graph

STRUCTOPT(poac::cmd::graph::Options, output_file);
