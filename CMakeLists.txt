cmake_minimum_required(VERSION 3.4)
project(covscript)
include_directories(include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -fPIE -s -O3")

add_executable(cs sources/standalone.cpp)
add_executable(cs_repl sources/repl.cpp)

add_library(test-extension SHARED tests/extension.cpp)

set_target_properties(test-extension PROPERTIES OUTPUT_NAME my_ext)
set_target_properties(test-extension PROPERTIES PREFIX "")
set_target_properties(test-extension PROPERTIES SUFFIX ".cse")

if (UNIX OR APPLE)
    target_link_libraries(cs dl)
    target_link_libraries(cs_repl dl)
endif ()