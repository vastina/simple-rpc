cmake_minimum_required(VERSION 3.9)

set(project_name rpc)
project(${project_name})

add_compile_options("-Wall" "-Wextra")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(project_dir "./code")
include_directories("${project_dir}/include")

file(GLOB SOURCES "${project_dir}/${project_name}/*.cpp")
add_library(code_objects STATIC ${SOURCES})

add_executable(${project_name}.out ${project_dir}/main.cpp)
target_link_libraries(${project_name}.out code_objects)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/)

add_subdirectory(${CMAKE_SOURCE_DIR}/testcase)
