cmake_minimum_required(VERSION 3.4)
project(covscript)
include_directories(include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -fPIE -s -O3")

add_executable(cs sources/covscript.cpp
        sources/zip/buffer_utils.cpp
        sources/zip/buffer_utils.hpp
        sources/zip/centraldir.cpp
        sources/zip/compat.hpp
        sources/zip/fs_utils.cpp
        sources/zip/fs_utils.hpp
        sources/zip/quicker_zip.cpp
        sources/zip/quicker_zip.hpp
        sources/zip/zip.hpp
        sources/zip/zipfile.cpp
        sources/zip/zipfile.hpp)
add_library(test-extension SHARED tests/extension.cpp)

target_link_libraries(cs sqlite3)
target_link_libraries(cs z)


if (UNIX OR APPLE)
    target_link_libraries(cs dl)
endif ()