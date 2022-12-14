# Headers including `poac/poac.hpp` require these libraries
set(POAC_HPP_LIBS
    fmt::fmt
    spdlog::spdlog
    poac_util_result
    poac_util_rustify
)

add_subdirectory(cmd)
add_subdirectory(core)
add_subdirectory(data)
add_subdirectory(util)
