cmake_minimum_required(VERSION 3.4)
project(covscript)
include_directories(include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -fPIE -s -O3")

add_executable(cs sources/standalone.cpp)
add_executable(cs_repl sources/repl.cpp)

add_library(regex-ext SHARED sources/extensions/regex.cpp)
add_library(darwin-ext SHARED sources/extensions/darwin.cpp)
add_library(sqlite-ext SHARED sources/extensions/sqlite.cpp)

add_library(test-extension SHARED tests/extension.cpp)

set_target_properties(regex-ext PROPERTIES OUTPUT_NAME regex)
set_target_properties(regex-ext PROPERTIES PREFIX "")
set_target_properties(regex-ext PROPERTIES SUFFIX ".cse")

set_target_properties(darwin-ext PROPERTIES OUTPUT_NAME darwin)
set_target_properties(darwin-ext PROPERTIES PREFIX "")
set_target_properties(darwin-ext PROPERTIES SUFFIX ".cse")

set_target_properties(sqlite-ext PROPERTIES OUTPUT_NAME sqlite)
set_target_properties(sqlite-ext PROPERTIES PREFIX "")
set_target_properties(sqlite-ext PROPERTIES SUFFIX ".cse")

set_target_properties(test-extension PROPERTIES OUTPUT_NAME my_ext)
set_target_properties(test-extension PROPERTIES PREFIX "")
set_target_properties(test-extension PROPERTIES SUFFIX ".cse")

target_link_libraries(sqlite-ext sqlite3)

if (UNIX OR APPLE)
    target_link_libraries(cs dl)
    target_link_libraries(cs_repl dl)
endif ()