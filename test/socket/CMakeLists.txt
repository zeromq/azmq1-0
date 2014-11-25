project(test_socket)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}
                                      ${ZeroMQ_LIBRARIES}
                                      aziomq)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

