#ifndef POAC_CORE_INFER_HPP
#define POAC_CORE_INFER_HPP

#include <string>
#include <unordered_map>
#include <functional>
#include <optional>

#include <poac/core/except.hpp>
#include <poac/opts.hpp>

namespace poac::core::infer {
    using main_arg = std::vector<std::string>;
    using main_ret = std::optional<except::Error>;
    using main_fun = std::function<main_ret(main_arg)>;

    const std::unordered_map<std::string, main_fun>
    opts_map{
        { "build",     opts::build::_main },
        { "cache",     opts::cache::_main },
        { "cleanup",   opts::cleanup::_main },
        { "graph",     opts::graph::_main },
        { "help",      opts::help::_main },
        { "--help",    opts::help::_main },
        { "-h",        opts::help::_main },
        { "init",      opts::init::_main },
        { "install",   opts::install::_main },
        { "new",       opts::_new::_main },
        { "publish",   opts::publish::_main },
        { "root",      opts::root::_main },
        { "run",       opts::run::_main },
        { "search",    opts::search::_main },
        { "test",      opts::test::_main },
        { "uninstall", opts::uninstall::_main },
        { "update",    opts::update::_main },
        { "version",   opts::version::_main },
        { "--version", opts::version::_main },
        { "-v",        opts::version::_main }
    };

    template <typename S, typename VS>
    main_ret
    exec(S&& cmd, VS&& arg) {
        try {
            return opts_map.at(cmd)(arg);
        }
        catch(std::out_of_range&) {
            return except::Error::InvalidFirstArg{};
        }
    }
}
#endif // !POAC_CORE_INFER_HPP
