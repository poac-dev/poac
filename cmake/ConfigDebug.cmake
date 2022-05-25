include_guard(GLOBAL)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address")
# https://stackoverflow.com/a/38297422
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC
