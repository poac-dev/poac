include_guard(GLOBAL)

add_library(termcolor2_color_mode)
target_sources(termcolor2_color_mode
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/termcolor2/color_mode.ixx
)
target_link_libraries(termcolor2_color_mode PRIVATE
    spdlog::spdlog
)

add_library(termcolor2_presets)
target_sources(termcolor2_presets
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/termcolor2/presets.ixx
)

add_library(termcolor2_to_color)
target_sources(termcolor2_to_color
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/termcolor2/to_color.ixx
)
target_link_libraries(termcolor2_to_color PRIVATE
    termcolor2_color_mode
    termcolor2_presets
)

add_library(termcolor2_to_color_extra)
target_sources(termcolor2_to_color_extra
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/termcolor2/to_color_extra.ixx
)
target_link_libraries(termcolor2_to_color_extra PRIVATE
    termcolor2_color_mode
    termcolor2_presets
)

add_library(termcolor2_literals)
target_sources(termcolor2_literals
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/termcolor2/literals.ixx
)
target_link_libraries(termcolor2_literals PRIVATE
    termcolor2_to_color
)

add_library(termcolor2)
target_sources(termcolor2
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/termcolor2/termcolor2.ixx
)
target_link_libraries(termcolor2 PRIVATE
    termcolor2_color_mode
    termcolor2_literals
    termcolor2_presets
    termcolor2_to_color
)

add_library(termcolor2_literals_extra)
target_sources(termcolor2_literals_extra
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/termcolor2/literals_extra.ixx
)
target_link_libraries(termcolor2_literals_extra PRIVATE
    termcolor2_to_color_extra
)
