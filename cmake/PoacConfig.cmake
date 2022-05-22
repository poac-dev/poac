include_guard(GLOBAL)
include(cmake/Helpers.cmake)

add_poac_definitions(POAC_VERSION="${PROJECT_VERSION}")
add_poac_definitions(TOML11_NO_ERROR_PREFIX)
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # undefined reference to symbol 'pthread_condattr_setclock@@GLIBC_2.3.3'
    if (APPLE)
        add_poac_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)
    else ()
        add_poac_definitions(_GNU_SOURCE BOOST_ASIO_HAS_STD_STRING_VIEW)
    endif ()
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_poac_options(-fdiagnostics-color -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_poac_options(/W4 /DBOOST_ASIO_HAS_STD_STRING_VIEW /bigobj /Zc:preprocessor)
else ()
    add_poac_options(-fdiagnostics-color -Wall -Wextra -pedantic-errors)
    if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
        add_poac_options(-Werror)
    endif ()
    if (CYGWIN OR MINGW)
        add_poac_options(-O3 -Wa,-mbig-obj)
    endif ()
endif ()

if ((MSVC OR MINGW) AND ENABLE_STATIC) # -DENABLE_STATIC=ON
    include(cmake/ConfigStatic.cmake)
elseif (CMAKE_BUILD_TYPE STREQUAL Debug) # -DCMAKE_BUILD_TYPE=Debug
    include(cmake/ConfigDebug.cmake)
elseif (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    include(cmake/ConfigRelease.cmake)
endif ()
