#ifndef POAC_CORE_BUILDER_ABSORB_HPP
#define POAC_CORE_BUILDER_ABSORB_HPP

#include <boost/predef.h>

namespace poac::core::builder::absorb {
#if BOOST_OS_WINDOWS
    const std::string binary_extension = ".exe";
    const std::string dynamic_lib_extension = ".dll";
    const std::string dynamic_lib_option = "-shared -fPIC";
#elif BOOST_OS_MACOS
    const std::string binary_extension = "";
    const std::string dynamic_lib_extension = ".dylib";
    const std::string dynamic_lib_option = "-dynamiclib";
#else
    const std::string binary_extension = "";
    const std::string dynamic_lib_extension = ".so";
    const std::string dynamic_lib_option = "-shared -fPIC";
#endif
} // end namespace
#endif // POAC_CORE_BUILDER_ABSORB_HPP
