#ifndef POAC_CORE_INFER_HPP
#define POAC_CORE_INFER_HPP

#include <string>
#include <unordered_map>
#include <functional>

#include "except.hpp"
#include "../opts.hpp"

namespace poac::core::infer {
    const std::unordered_map<std::string, std::function<int(std::vector<std::string>)>>
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
    int execute(S&& cmd, VS&& arg) {
        namespace except = core::except;
        try {
            return opts_map.at(cmd)(arg);
        }
        catch(std::out_of_range&) {
            throw except::invalid_first_arg("Invalid argument");
        }
    }
}
#endif // !POAC_CORE_INFER_HPP
