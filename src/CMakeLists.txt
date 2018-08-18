add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_TIMER_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${CURL_LIBRARIES}
        ${YAML_CPP_LIBRARIES}
        )
install(TARGETS poac DESTINATION bin)
