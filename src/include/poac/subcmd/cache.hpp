#ifndef POAC_SUBCMD_CACHE_HPP
#define POAC_SUBCMD_CACHE_HPP

#include <iostream>
#include <string>
#include <sstream>
//#include <optional>

#include <boost/optional.hpp>

#include "../core/except.hpp"
#include "../io/cli.hpp"
#include "../io/network.hpp"


namespace poac::subcmd { struct cache {
        static const std::string summary() { return "Manipulate cache files."; }
        static const std::string options() { return "<command>"; }

        template <typename VS>
        void operator()(VS&& vs) { _main(vs); }
        template <typename VS>
        void _main([[maybe_unused]] VS&& vs) {
            // root
            // list
            // clean
        }
    };} // end namespace
#endif // !POAC_SUBCMD_CACHE_HPP
