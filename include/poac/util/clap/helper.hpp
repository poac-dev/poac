#ifndef CLAP_HELPER_HPP
#define CLAP_HELPER_HPP

#include <poac/util/clap/app.hpp>

namespace clap {
    arg opt(const std::string& name, const std::string& help) {
        return arg(name).long_(name).help(help);
    }
} // end namespace clap

#endif	// !CLAP_HELPER_HPP
