cmake_minimum_required(VERSION 3.1)
project(test_cmdo CXX)
enable_testing()

set(GTEST_DIR thirdparty/googletest)
add_subdirectory(${GTEST_DIR})

set(SOURCE_FILES src/main.cpp
    src/cmdo/StringUtilTest.cpp
    src/cmdo/StringUtilTest.h
    src/cmdo/CmdLineOptionsTest.cpp
    src/cmdo/CmdLineOptionsTest.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE
    src
    ${CMAKE_SOURCE_DIR}/lib/src
    ${GTEST_DIR}/googletest/include)

set_target_properties(${PROJECT_NAME} PROPERTIES
    COMPILE_FLAGS "-std=c++11")

target_link_libraries(${PROJECT_NAME} PRIVATE cmdo_static gtest_main)

add_test(AllTests ${PROJECT_NAME})