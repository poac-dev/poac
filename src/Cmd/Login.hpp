#pragma once

// external
#include <structopt/app.hpp>

// internal
#include "../Util/Result.hpp"
#include "../Util/Rustify.hpp"

namespace poac::cmd::login {

struct Options : structopt::sub_command {
  /// API Token obtained on poac.dev
  String api_token;
};

[[nodiscard]] auto check_token(StringRef api_token) -> Result<void>;

[[nodiscard]] auto exec(const Options& opts) -> Result<void>;

} // namespace poac::cmd::login

STRUCTOPT(poac::cmd::login::Options, api_token);
