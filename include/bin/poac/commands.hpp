#ifndef BIN_POAC_COMMANDS_HPP
#define BIN_POAC_COMMANDS_HPP

#include <bin/poac/commands/init.hpp>
#include <bin/poac/commands/new.hpp>

#include <string_view>
#include <optional>
#include <vector>
#include <poac/util/clap/clap.hpp>

namespace bin::poac::commands {

std::vector<clap::subcommand>
builtin() noexcept {
    return {
        init::cli,
        _new::cli,
    };
}

using arg1_type = std::future<std::optional<::poac::io::config::Config>>;
using arg2_type = std::vector<std::string>;
using ret_type = std::optional<::poac::core::except::Error>;
using fn_type = std::function<ret_type(arg1_type, arg2_type)>;

std::optional<fn_type>
builtin_exec(std::string_view cmd) noexcept {
    const std::unordered_map<std::string_view, fn_type> f{
        { "init", init::exec },
        { "new",  _new::exec },
    };

    try {
        return f.at(cmd);
    } catch (std::out_of_range&) {
        return std::nullopt;
    }
}

} // namespace bin::poac

#endif // !BIN_POAC_COMMANDS_HPP
