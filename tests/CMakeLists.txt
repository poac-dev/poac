# Erase the compile options of Boost::ut because GCC provides many warnings.
# ref: https://github.com/boost-ext/ut/blob/67b136c0267a54248d05fcba63905ff5e6abf0b7/CMakeLists.txt#L83-L115
# CMake states:
#   When target dependencies are specified using target_link_libraries(),
#   CMake will read this property from all target dependencies to determine the build properties of the consumer.
# So, we need to erase the variable.
# ref about `INTERFACE_COMPILE_OPTIONS`:
#   https://cmake.org/cmake/help/latest/prop_tgt/INTERFACE_COMPILE_OPTIONS.html
set_property(TARGET ut PROPERTY INTERFACE_COMPILE_OPTIONS)

add_subdirectory(core)
add_subdirectory(util)
