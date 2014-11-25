add_subdirectory(message)
add_subdirectory(context_ops)
add_subdirectory(socket_ops)
add_subdirectory(socket)
add_subdirectory(thread)

add_test(message ${CMAKE_CURRENT_BINARY_DIR}/message/test_message)
add_test(message ${CMAKE_CURRENT_BINARY_DIR}/context_ops/test_context_ops)
add_test(message ${CMAKE_CURRENT_BINARY_DIR}/socket_ops/test_socket_ops)
add_test(message ${CMAKE_CURRENT_BINARY_DIR}/socket/test_socket)
add_test(message ${CMAKE_CURRENT_BINARY_DIR}/thread/test_thread)



