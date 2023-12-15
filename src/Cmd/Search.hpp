#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::search {

struct Options : structopt::sub_command {
  /// Package name to search
  String package_name;
};

[[nodiscard]] auto exec(const Options& opts) -> Result<void>;

} // namespace poac::cmd::search

STRUCTOPT(poac::cmd::search::Options, package_name);
