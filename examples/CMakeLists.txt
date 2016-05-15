cmake_minimum_required(VERSION 3.1)
project(cmdo_examples CXX)

add_executable(example src/example.cpp)
target_include_directories(example PRIVATE
      src
      ${CMAKE_SOURCE_DIR}/lib/src)

set_target_properties(example PROPERTIES
    COMPILE_FLAGS "-std=c++11")

target_link_libraries(example PRIVATE cmdo_static)

add_dependencies(example cmdo_static)