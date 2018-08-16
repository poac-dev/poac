add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        ${Boost_LIBRARIES}
        ${CURL_LIBRARIES}
        ${YAML_CPP_LIBRARIES}
#        git2
        )
message(STATUS "boost: ${Boost_LIBRARIES}")
message(STATUS "curl: ${CURL_LIBRARIES}")
message(STATUS "yaml: ${YAML_CPP_LIBRARIES}")

install(TARGETS poac DESTINATION bin)
