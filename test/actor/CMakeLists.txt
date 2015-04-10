project(test_actor)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT}
                                      ${ZeroMQ_LIBRARIES}
                                      ${Boost_LIBRARIES}
                                      ${ADDITIONAL_LIBS})

add_catch_test(${PROJECT_NAME})
