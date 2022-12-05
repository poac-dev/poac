#ifndef POAC_CMD_CLEAN_HPP_
#define POAC_CMD_CLEAN_HPP_

// external
#include <structopt/app.hpp>

// internal
#include "poac/cmd/build.hpp"
#include "poac/poac.hpp"

namespace poac::cmd::clean {

struct Options : structopt::sub_command {
  /// Whether or not to clean release artifacts
  Option<bool> release = false;
  /// Clean artifacts of the specified profile
  Option<String> profile;
};

[[nodiscard]] Result<void> clean(const Options& opts);

[[nodiscard]] Result<void> exec(const Options& opts);

} // namespace poac::cmd::clean

STRUCTOPT(poac::cmd::clean::Options, release, profile);

#endif // POAC_CMD_CLEAN_HPP_
