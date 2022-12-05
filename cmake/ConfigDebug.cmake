include_guard(GLOBAL)

# ref: https://github.com/google/sanitizers/wiki/AddressSanitizer#using-addresssanitizer
set(CMAKE_CXX_FLAGS_DEBUG "-g -O1 -fsanitize=address -fno-omit-frame-pointer")
# https://stackoverflow.com/a/38297422
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC

if (POAC_CLANG_TIDY)
    # Enable clang-tidy
    set(CMAKE_CXX_CLANG_TIDY clang-tidy) # -fix
endif ()
