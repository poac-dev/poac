#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::lint {

struct Options : structopt::sub_command {
  // Dummy option: DO NOT USE
  Option<bool> dummy = false;
  // FIXME(ken-matsui): this is structopt limitation; I'd remove this.
};

[[nodiscard]] auto exec(const Options&)
    -> Result<void>;

} // namespace poac::cmd::lint

STRUCTOPT(poac::cmd::lint::Options, dummy);
