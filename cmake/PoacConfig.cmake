include_guard(GLOBAL)

target_compile_definitions(poac PRIVATE POAC_VERSION="${PROJECT_VERSION}")
if (NOT MSVC)
    # undefined reference to symbol 'pthread_condattr_setclock@@GLIBC_2.3.3'
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    if (APPLE)
        target_compile_definitions(poac PRIVATE BOOST_BEAST_USE_STD_STRING_VIEW)
    else ()
        target_compile_definitions(poac PRIVATE _GNU_SOURCE BOOST_ASIO_HAS_STD_STRING_VIEW)
    endif ()
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(poac PRIVATE -fdiagnostics-color -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(poac PRIVATE /W4 /DBOOST_ASIO_HAS_STD_STRING_VIEW /bigobj /Zc:preprocessor)
else ()
    target_compile_options(poac PRIVATE -fdiagnostics-color -Wall -Wextra -pedantic-errors)
    if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
        target_compile_options(poac PRIVATE -Werror)
    endif ()
    if (CYGWIN OR MINGW)
        target_compile_options(poac PRIVATE -O3 -Wa,-mbig-obj)
    endif ()
endif ()

if ((MSVC OR MINGW) AND ENABLE_STATIC) # -DENABLE_STATIC=ON
    include(cmake/ConfigStatic.cmake)
elseif (CMAKE_BUILD_TYPE STREQUAL Debug) # -DCMAKE_BUILD_TYPE=Debug
    include(cmake/ConfigDebug.cmake)
elseif (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    include(cmake/ConfigRelease.cmake)
endif ()
