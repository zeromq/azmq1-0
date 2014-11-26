project(test_message)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}
                                      ${ZeroMQ_LIBRARIES})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

