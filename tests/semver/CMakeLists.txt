set(TEST_NAMES
        interval
        lexer
#        parser
        token
        version
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

if (APPLE)
    # If macOS version less than mojave, set compiler to homebrew llvm@7.
    execute_process(
        COMMAND bash -c "sw_vers | head -n 2 | tail -n 1 | cut -f 2 | cut -f 2 -d \".\""
        OUTPUT_VARIABLE OSX_VERSION
    )
    if (${OSX_VERSION} LESS 14)
        set(HOMEBREW_LLVM_PATH /usr/local/opt/llvm@7)
        if (NOT EXISTS ${HOMEBREW_LLVM_PATH})
            message(FATAL_ERROR "You need llvm@7 to build. Please execute `brew install llvm@7` to install it.")
        endif ()
        message(STATUS "Set compiler to homebrew llvm@7.")
        set(CMAKE_CXX_COMPILER "${HOMEBREW_LLVM_PATH}/bin/clang++")
        set(HOMEBREW_LLVM_INCLUDE_PATH ${HOMEBREW_LLVM_PATH}/include)
        set(HOMEBREW_LLVM_LIBRARY_PATH ${HOMEBREW_LLVM_PATH}/lib)
        unset(HOMEBREW_LLVM_PATH)
    endif ()
endif ()

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)
    target_compile_options(${TEST_NAME} PRIVATE -g -O0)
    if (CMAKE_BUILD_TYPE STREQUAL Coverage)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif ()
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS} ${HOMEBREW_LLVM_INCLUDE_PATH})
    target_link_directories(${TEST_NAME} PRIVATE ${HOMEBREW_LLVM_LIBRARY_PATH})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endforeach ()
