add_library(aziomq SHARED error.cpp
                          context_ops.cpp
                          socket_service.cpp)

target_link_libraries(aziomq ${Boost_LIBRARIES}
                             ${ZeroMQ_LIBRARIES})

install(TARGETS aziomq ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION lib)
