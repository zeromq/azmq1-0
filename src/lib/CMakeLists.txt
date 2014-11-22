set(SOURCES
    error.cpp
    context_ops.cpp
    socket_service.cpp
    thread_service.cpp
)

if (${CMAKE_CXX_PLATFORM_ID} STREQUAL "Windows")
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
endif()

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    ${ZeroMQ_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib
)
