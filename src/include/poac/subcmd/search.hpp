#ifndef POAC_SUBCMD_SEARCH_HPP
#define POAC_SUBCMD_SEARCH_HPP

#include <iostream>
#include <string>


namespace poac::subcmd { struct search {
    static const std::string summary() { return "Search for packages in poacpm."; }
    static const std::string options() { return "<pkg-name>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main([[maybe_unused]] VS&& vs) {}
};} // end namespace
#endif // !POAC_SUBCMD_SEARCH_HPP