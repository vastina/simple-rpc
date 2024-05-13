set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
include_directories("./include")


add_executable(basic_client  ./basic/client.cpp)
target_link_libraries(basic_client code_objects)
add_executable(basic_server  ./basic/server.cpp)
target_link_libraries(basic_server code_objects)