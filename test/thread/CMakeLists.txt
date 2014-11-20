#include_directories("${PROJECT_SOURCE_DIR}/src")
add_executable(test_thread main.cpp)
target_link_libraries(test_thread ${Boost_LIBRARIES}
                                  ${ZeroMQ_LIBRARIES}
                                  aziomq)
