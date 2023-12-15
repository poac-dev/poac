#include "Login.hpp"

// std
#include <fstream>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "../Config.hpp"
#include "../Util/Format.hpp"
#include "../Util/Log.hpp"
#include "../Util/Net.hpp"
#include "../Util/ResultMacros.hpp"

namespace poac::cmd::login {

using InvalidAPIToken = Error<"invalid API token provided">;
using FailedToLogIn = Error<"failed to log in; API token might be incorrect">;

[[nodiscard]] auto check_token(StringRef api_token) -> Result<void> {
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

[[nodiscard]] auto exec(const Options& opts) -> Result<void> {
  Try(check_token(opts.api_token));

  // Write API Token to `~/.poac/credentials` as TOML
  spdlog::trace("Exporting the api_token to `{}`", config::cred_file);
  std::ofstream ofs(config::cred_file, std::ofstream::trunc);
  ofs << format("[registry]\ntoken = \"{}\"\n", opts.api_token);

  log::status("Login", "token for `{}` saved", "poac.dev");
  return Ok();
}

} // namespace poac::cmd::login
