#ifndef POAC_CMD_LOGIN_HPP
#define POAC_CMD_LOGIN_HPP

// std
#include <fstream>
#include <string>
#include <string_view>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>
#include <spdlog/spdlog.h>
#include <structopt/app.hpp>

// internal
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/net.hpp>
#include <poac/config.hpp>

namespace poac::cmd::login {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    struct Options: structopt::sub_command {
        /// API Token obtained on poac.pm
        std::string api_token;
    };

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using InvalidAPIToken =
            error<"invalid API token provided">;

        using FailedToLogIn =
            error<"failed to log in; API token might be incorrect">;
    };

    [[nodiscard]] anyhow::result<void>
    check_token(std::string_view api_token) {
        spdlog::trace("Checking if api_token has 32 length");
        if (api_token.size() != 32) {
            return anyhow::failure<Error::InvalidAPIToken>();
        }
        spdlog::trace("Checking if api_token exists");
        if (!util::net::api::login(api_token).unwrap_or(false)) {
            return anyhow::failure<Error::FailedToLogIn>();
        }
        return mitama::success();
    }

    [[nodiscard]] anyhow::result<void>
    exec(const Options& opts) {
        MITAMA_TRY(check_token(opts.api_token));

        // Write API Token to `~/.poac/credentials` as TOML
        spdlog::trace("Exporting the api_token to `{}`", config::path::credentials);
        std::ofstream ofs(config::path::credentials, std::ofstream::trunc);
        ofs << fmt::format("[registry]\ntoken = \"{}\"\n", opts.api_token);

        using termcolor2::color_literals::operator""_bold_green;
        spdlog::info(
            "{:>25} token for `{}` saved",
            "Login"_bold_green,
            "poac.pm"
        );
        return mitama::success();
    }
} // end namespace

STRUCTOPT(poac::cmd::login::Options, api_token);

#endif // !POAC_CMD_LOGIN_HPP
