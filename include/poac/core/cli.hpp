#ifndef POAC_CORE_CLI_HPP
#define POAC_CORE_CLI_HPP

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>

#include <poac/core/except.hpp>
#include <poac/io/config.hpp>
#include <poac/opts.hpp>

namespace poac::core::cli {
    using arg1_type = std::optional<io::config::Config>;
    using arg2_type = std::vector<std::string>;
    using ret_type = std::optional<except::Error>;
    using fn_type = std::function<ret_type(arg1_type, arg2_type)>;

    const std::unordered_map<std::string_view, fn_type>
    opts_map{
        { "build",     opts::build::exec },
        { "cache",     opts::cache::exec },
        { "cleanup",   opts::cleanup::exec },
        { "graph",     opts::graph::exec },
        { "help",      opts::help::exec },
        { "--help",    opts::help::exec },
        { "-h",        opts::help::exec },
        { "init",      opts::init::exec },
        { "install",   opts::install::exec },
        { "new",       opts::_new::exec },
        { "publish",   opts::publish::exec },
        { "root",      opts::root::exec },
        { "run",       opts::run::exec },
        { "search",    opts::search::exec },
        { "test",      opts::test::exec },
        { "uninstall", opts::uninstall::exec },
        { "update",    opts::update::exec },
        { "version",   opts::version::exec },
        { "--version", opts::version::exec },
        { "-v",        opts::version::exec }
    };

    [[nodiscard]] ret_type
    exec(std::string_view cmd, std::vector<std::string>&& args) {
        try {
            return opts_map.at(cmd)(io::config::load(), std::move(args));
        }
        catch(std::out_of_range&) {
            return except::Error::InvalidFirstArg;
        }
    }
}
#endif // !POAC_CORE_CLI_HPP
