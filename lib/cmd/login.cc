// std
#include <fstream>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "poac/cmd/login.hpp"
#include "poac/config.hpp"
#include "poac/util/net.hpp"

namespace poac::cmd::login {

[[nodiscard]] auto check_token(StringRef api_token) -> Result<void> {
  spdlog::trace("Checking if api_token has 32 length");
  if (api_token.size() != 32) {
    return Err<InvalidAPIToken>();
  }
  spdlog::trace("Checking if api_token exists");
  if (!util::net::api::login(api_token).unwrap_or(false)) {
    return Err<FailedToLogIn>();
  }
  return Ok();
}

[[nodiscard]] auto exec(const Options& opts) -> Result<void> {
  Try(check_token(opts.api_token));

  // Write API Token to `~/.poac/credentials` as TOML
  spdlog::trace("Exporting the api_token to `{}`", config::path::cred_file);
  std::ofstream ofs(config::path::cred_file, std::ofstream::trunc);
  ofs << format("[registry]\ntoken = \"{}\"\n", opts.api_token);

  log::status("Login", "token for `{}` saved", "poac.pm");
  return Ok();
}

} // namespace poac::cmd::login
