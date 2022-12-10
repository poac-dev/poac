#ifndef POAC_CMD_LINT_HPP_
#define POAC_CMD_LINT_HPP_

// external
#include <structopt/app.hpp>

// internal
#include "poac/config.hpp"
#include "poac/poac.hpp"

namespace poac::cmd::lint {

struct Options : structopt::sub_command {
  // Dummy option: DO NOT USE
  Option<bool> dummy = false;
  // FIXME(ken-matsui): this is structopt limitation; I'd remove this.
};

[[nodiscard]] Fn exec([[maybe_unused]] const Options& opts)->Result<void>;

} // namespace poac::cmd::lint

STRUCTOPT(poac::cmd::lint::Options, dummy);

#endif // POAC_CMD_LINT_HPP_
