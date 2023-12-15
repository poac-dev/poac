#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "../Config.hpp"
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::fmt {

struct Options : structopt::sub_command {
  /// Perform only checks
  Option<bool> check = false;
};

inline constexpr Arr<StringRef, 3> EXCLUDES{
    config::POAC_OUT, "build", "cmake-build-debug" // CLion
};

[[nodiscard]] auto exec(const Options& opts) -> Result<void>;

} // namespace poac::cmd::fmt

STRUCTOPT(poac::cmd::fmt::Options, check);
