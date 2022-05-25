#ifndef POAC_CMD_RUN_HPP_
#define POAC_CMD_RUN_HPP_

// external
#include <structopt/app.hpp>

// internal
#include "poac/poac.hpp"

namespace poac::cmd::run {

struct Options : structopt::sub_command {
  /// Build artifacts in release mode, with optimizations
  Option<bool> release = false;
};

[[nodiscard]] Result<void>
exec(const Options& opts);

} // namespace poac::cmd::run

STRUCTOPT(poac::cmd::run::Options, release);

#endif // POAC_CMD_RUN_HPP_
