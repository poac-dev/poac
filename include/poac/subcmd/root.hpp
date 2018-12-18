#ifndef POAC_SUBCMD_ROOT_HPP
#define POAC_SUBCMD_ROOT_HPP

#include <iostream>
#include <string>
#include <cstdlib>


namespace poac::subcmd {
    struct root {
        static const std::string summary() { return "Display the root installation directory."; }
        static const std::string options() { return "<Nothing>"; }
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()([[maybe_unused]] VS&& vs) {
            std::cout << POAC_PROJECT_ROOT << std::endl;
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_ROOT_HPP