add_subdirectory(message)
add_subdirectory(context_ops)
add_subdirectory(socket_ops)
add_subdirectory(socket)
add_subdirectory(thread)

add_test(message ${CMAKE_CURRENT_BINARY_DIR}/test_context_ops)
add_test(message ${CMAKE_CURRENT_BINARY_DIR}/test_socket_ops)
add_test(message ${CMAKE_CURRENT_BINARY_DIR}/test_socket)



