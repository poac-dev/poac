#ifndef STROITE_UTILS_ABSORPER_HPP
#define STROITE_UTILS_ABSORPER_HPP


namespace poac::core::stroite::utils::absorper {
#ifdef __APPLE__
    const std::string binary_extension = "";
    const std::string dynamic_lib_extension = ".dylib";
    const std::string dynamic_lib_option = "-dynamiclib";
#elif defined(_WIN32)
    const std::string binary_extension = ".exe";
    const std::string dynamic_lib_extension = ".dll";
    const std::string dynamic_lib_option = "-shared -fPIC";
#else
    const std::string binary_extension = "";
    const std::string dynamic_lib_extension = ".so";
    const std::string dynamic_lib_option = "-shared -fPIC";
#endif
} // end namespace
#endif // STROITE_UTILS_ABSORPER_HPP
