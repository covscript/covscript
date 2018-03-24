cmake_minimum_required(VERSION 3.4)
project(covscript)
include_directories(include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-s -O3")

add_library(covscript SHARED sources/covscript.cpp)
add_executable(cs sources/standalone.cpp)
add_executable(cs_repl sources/repl.cpp)

add_library(test-extension SHARED tests/extension.cpp)
add_executable(test-covscript tests/test.cpp)

target_link_libraries(test-covscript covscript)

set_target_properties(test-extension PROPERTIES OUTPUT_NAME my_ext)
set_target_properties(test-extension PROPERTIES PREFIX "")
set_target_properties(test-extension PROPERTIES SUFFIX ".cse")

if (UNIX OR APPLE)
    target_link_libraries(covscript dl)
    target_link_libraries(cs dl)
    target_link_libraries(cs_repl dl)
    target_link_libraries(test-covscript dl)
endif ()