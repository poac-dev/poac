#ifndef POAC_CORE_STROITE_UTILS_ABSORB_HPP
#define POAC_CORE_STROITE_UTILS_ABSORB_HPP


namespace poac::core::stroite::utils::absorb {
#ifdef _WIN32
    const std::string binary_extension = ".exe";
    const std::string dynamic_lib_extension = ".dll";
    const std::string dynamic_lib_option = "-shared -fPIC";
#elif __APPLE__
    const std::string binary_extension = "";
    const std::string dynamic_lib_extension = ".dylib";
    const std::string dynamic_lib_option = "-dynamiclib";
#else
    const std::string binary_extension = "";
    const std::string dynamic_lib_extension = ".so";
    const std::string dynamic_lib_option = "-shared -fPIC";
#endif
} // end namespace
#endif // POAC_CORE_STROITE_UTILS_ABSORB_HPP
