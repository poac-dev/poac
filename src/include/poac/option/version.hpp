#ifndef POAC_OPTION_VERSION_HPP
#define POAC_OPTION_VERSION_HPP

#include <iostream>


namespace poac::option { struct version {
    static const std::string summary() { return "Show the current poac version."; }
    static const std::string options() { return "<Nothing>"; }

    template <typename VS>
    void operator()([[maybe_unused]] VS&& vs) { std::cout << POAC_VERSION << std::endl; }
};} // end namespace
#endif // !POAC_OPTION_VERSION_HPP