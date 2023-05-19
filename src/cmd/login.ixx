module;

// std
#include <fstream>

// external
#include <structopt/app.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "../util/result-macros.hpp"

export module poac.cmd.login;

import poac.config;
import poac.util.format;
import poac.util.log;
import poac.util.net;
import poac.util.result;
import poac.util.rustify;

namespace poac::cmd::login {

export struct Options : structopt::sub_command {
  /// API Token obtained on poac.dev
  String api_token;
};

using InvalidAPIToken = Error<"invalid API token provided">;
using FailedToLogIn = Error<"failed to log in; API token might be incorrect">;

export [[nodiscard]] auto check_token(StringRef api_token)->Result<void> {
  spdlog::trace("Checking if api_token has 32 length");
  if (api_token.size() != 32) {
    return Err<InvalidAPIToken>();
  }
  // TODO(ken-matsui): Implement login API
  //  spdlog::trace("Checking if api_token exists");
  //  if (!util::net::api::login(api_token).unwrap_or(false)) {
  //    return Err<FailedToLogIn>();
  //  }
  return Ok();
}

export [[nodiscard]] auto exec(const Options& opts)->Result<void> {
  Try(check_token(opts.api_token));

  // Write API Token to `~/.poac/credentials` as TOML
  spdlog::trace("Exporting the api_token to `{}`", config::cred_file);
  std::ofstream ofs(config::cred_file, std::ofstream::trunc);
  ofs << format("[registry]\ntoken = \"{}\"\n", opts.api_token);

  log::status("Login", "token for `{}` saved", "poac.dev");
  return Ok();
}

} // namespace poac::cmd::login

STRUCTOPT(poac::cmd::login::Options, api_token);
