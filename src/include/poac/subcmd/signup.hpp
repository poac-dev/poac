#ifndef POAC_SUBCMD_SIGNUP_HPP
#define POAC_SUBCMD_SIGNUP_HPP

#include <iostream>


namespace poac::subcmd { struct signup {
    static const std::string summary() { return "Create new account for poacpm."; }
    static const std::string options() { return "[<pkg-names>]"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main([[maybe_unused]] VS&& vs) {}
};} // end namespace
#endif // !POAC_SUBCMD_SIGNUP_HPP