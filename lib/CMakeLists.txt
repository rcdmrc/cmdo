cmake_minimum_required(VERSION 3.1)
project(libcmdo CXX)

set(SOURCE_FILES
    src/cmdo/CmdLineOptions.cpp
    src/cmdo/CmdLineOptions.h
    src/cmdo/StringUtil.h)

set(TARGET_STATIC cmdo_static)
add_library(${TARGET_STATIC} STATIC ${SOURCE_FILES})
target_include_directories(${TARGET_STATIC} PRIVATE src)
set_target_properties(${TARGET_STATIC} PROPERTIES
    COMPILE_FLAGS "-fPIC")

install(DIRECTORY src/cmdo/ DESTINATION include/cmdo
    FILES_MATCHING PATTERN "*.h")

install(TARGETS ${TARGET_STATIC}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if (BUILD_SHARED_LIBS)
  set(TARGET_SHARED cmdo_shared)
  add_library(${TARGET_SHARED} SHARED ${SOURCE_FILES})
  target_include_directories(${TARGET_SHARED} PRIVATE
      libcmdo/src)
  set_target_properties(${TARGET_SHARED} PROPERTIES
      COMPILE_FLAGS "-fPIC")
endif (BUILD_SHARED_LIBS)
