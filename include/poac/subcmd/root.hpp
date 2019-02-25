#ifndef POAC_SUBCMD_ROOT_HPP
#define POAC_SUBCMD_ROOT_HPP

#include <iostream>
#include <string>
#include <type_traits>
#include <cstdlib>

namespace poac::subcmd {
    struct root {
        static std::string summary() {
            return "Display the root installation directory";
        }
        static std::string options() {
            return "<Nothing>";
        }
        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int operator()([[maybe_unused]] VS&& vs) {
            std::cout << POAC_PROJECT_ROOT << std::endl;
            return EXIT_SUCCESS;
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_ROOT_HPP
