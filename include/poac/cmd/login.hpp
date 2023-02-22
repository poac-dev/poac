#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::cmd::login {

struct Options : structopt::sub_command {
  /// API Token obtained on poac.dev
  String api_token;
};

[[nodiscard]] Fn check_token(StringRef api_token)->Result<void>;

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::login

STRUCTOPT(poac::cmd::login::Options, api_token);
