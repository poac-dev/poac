file(GLOB_RECURSE SOURCES include/*)
add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
target_link_libraries(${PROJECT_NAME}
        ${Boost_LIBRARIES}
        ${CURL_LIBRARIES}
        )
install(TARGETS poac DESTINATION bin)
