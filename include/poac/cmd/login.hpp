#ifndef POAC_CMD_LOGIN_HPP_
#define POAC_CMD_LOGIN_HPP_

// external
#include <structopt/app.hpp>

// internal
#include "poac/poac.hpp"

namespace poac::cmd::login {

struct Options : structopt::sub_command {
  /// API Token obtained on poac.pm
  String api_token;
};

using InvalidAPIToken = Error<"invalid API token provided">;
using FailedToLogIn = Error<"failed to log in; API token might be incorrect">;

[[nodiscard]] Result<void> check_token(StringRef api_token);

[[nodiscard]] Result<void> exec(const Options& opts);

} // namespace poac::cmd::login

STRUCTOPT(poac::cmd::login::Options, api_token);

#endif // POAC_CMD_LOGIN_HPP_
