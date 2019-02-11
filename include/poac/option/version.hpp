#ifndef POAC_OPTION_VERSION_HPP
#define POAC_OPTION_VERSION_HPP

#include <iostream>
#include <cstdlib>


namespace poac::option {
    struct version {
        static const std::string summary() {
            return "Show the current poac version";
        }
        static const std::string options() {
            return "<Nothing>";
        }
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int operator()([[maybe_unused]] VS&& argv) {
            std::cout << POAC_VERSION << std::endl;
            return EXIT_SUCCESS;
        }
    };
} // end namespace
#endif // !POAC_OPTION_VERSION_HPP