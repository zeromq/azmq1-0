project(test_signal)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}
                                      ${ZeroMQ_LIBRARIES})

add_catch_test(${PROJECT_NAME})
