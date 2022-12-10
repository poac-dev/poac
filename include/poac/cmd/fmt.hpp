#ifndef POAC_CMD_FMT_HPP_
#define POAC_CMD_FMT_HPP_

// external
#include <structopt/app.hpp>

// internal
#include "poac/poac.hpp"

namespace poac::cmd::fmt {

struct Options : structopt::sub_command {
  /// Perform only checks
  Option<bool> check = false;

  /// Format a drogon project
  Option<bool> drogon = false;
};

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::fmt

STRUCTOPT(poac::cmd::fmt::Options, check, drogon);

#endif // POAC_CMD_FMT_HPP_
