# Headers including `poac/poac.hpp` require these libraries
set(POAC_HPP_LIBS
    fmt::fmt
    mitama-cpp-result::mitama-cpp-result
    spdlog::spdlog
)

#add_subdirectory(cmd)
add_subdirectory(core)
#add_subdirectory(data)
add_subdirectory(util)
