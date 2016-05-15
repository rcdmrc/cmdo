cmake_minimum_required(VERSION 3.1)
project(cmdo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "${COMPILE_FLAGS} -O2 -g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${COMPILE_FLAGS} -O2 -NDEBUG -Wall")
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

add_subdirectory(lib)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif(BUILD_TESTS)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)