//
// Summary: Search poacpm for packages.
// Options:
//
#ifndef POAC_SUBCMD_SEARCH_HPP
#define POAC_SUBCMD_SEARCH_HPP

#include <iostream>
#include <string>

namespace poac::subcmd { struct search {
    static const std::string summary() { return "Search poacpm for packages."; }
    static const std::string options() { return "未定"; }
};}}
#endif