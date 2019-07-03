#ifndef POAC_CORE_CLI_HPP
#define POAC_CORE_CLI_HPP

#include <string>
#include <unordered_map>
#include <functional>
#include <optional>

#include <poac/core/except.hpp>
#include <poac/opts.hpp>

namespace poac::core::cli {
    using exec_arg = std::vector<std::string>;
    using exec_ret = std::optional<except::Error>;
    using exec_fn = std::function<exec_ret(exec_arg)>;

    const std::unordered_map<std::string, exec_fn>
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

    template <typename S, typename VS>
    exec_ret exec(S&& cmd, VS&& arg) {
        try {
            return opts_map.at(cmd)(arg);
        }
        catch(std::out_of_range&) {
            return except::Error::InvalidFirstArg{};
        }
    }
}
#endif // !POAC_CORE_CLI_HPP
