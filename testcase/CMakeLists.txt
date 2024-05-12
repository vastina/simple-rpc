set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)

set(test_dir1 "./test_name")

set(test_name "test_name")
add_executable(${test_name} ${test_dir1}/test_name.cpp)
target_link_libraries(${test_name} code_objects)
