cmake_minimum_required(VERSION 3.4)
project(covscript)
include_directories(include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -fPIE -s -O3")

add_executable(cs sources/covscript.cpp)
add_library(test-extension SHARED tests/extension.cpp)

target_link_libraries(cs sqlite3)

if (UNIX OR APPLE)
    target_link_libraries(cs dl)
endif ()