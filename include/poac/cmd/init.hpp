#ifndef POAC_CMD_INIT_HPP_
#define POAC_CMD_INIT_HPP_

// external
#include <structopt/app.hpp>

// internal
#include "poac/poac.hpp"

namespace poac::cmd::init {

struct Options : structopt::sub_command {
  /// Use a binary (application) template [default]
  Option<bool> bin = false;
  /// Use a library template
  Option<bool> lib = false;
};

using AlreadyInitialized = Error<"cannot initialize an existing poac package">;

[[nodiscard]] Fn init(const Options& opts, StringRef package_name)
    ->Result<void>;

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::init

STRUCTOPT(poac::cmd::init::Options, bin, lib);

#endif // POAC_CMD_INIT_HPP_
