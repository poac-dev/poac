#ifndef POAC_SUBCMD_SIGNIN_HPP
#define POAC_SUBCMD_SIGNIN_HPP

#include <iostream>


namespace poac::subcmd { struct signin {
    static const std::string summary() { return "Login to poacpm."; }
    static const std::string options() { return "[<pkg-names>]"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main([[maybe_unused]] VS&& vs) {}
};} // end namespace
#endif // !POAC_SUBCMD_SIGNIN_HPP