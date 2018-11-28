cmake_minimum_required(VERSION 3.4)
project(covscript)
include_directories(../include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-s -O3")

add_executable(cs
    compiler/codegen.cpp
    compiler/compiler.cpp
    compiler/lexer.cpp
    compiler/parser.cpp
    instance/extension.cpp
    instance/instance.cpp
    instance/runtime.cpp
    instance/statement.cpp
    covscript.cpp
    standalone.cpp
)

if (UNIX OR APPLE)
    target_link_libraries(cs dl)
endif ()