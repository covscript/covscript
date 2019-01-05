cmake_minimum_required(VERSION 3.4)
project(covscript)
include_directories(include)

set(CMAKE_CXX_STANDARD 11)

if (MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /EHsc /std:c++14 /utf-8 /w")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-fPIC -s -O3")
else ()
    set(CMAKE_CXX_FLAGS "-fPIC -O3")
endif ()

add_library(covscript STATIC
        sources/compiler/codegen.cpp
        sources/compiler/compiler.cpp
        sources/compiler/lexer.cpp
        sources/compiler/parser.cpp
        sources/instance/extension.cpp
        sources/instance/instance.cpp
        sources/instance/runtime.cpp
        sources/instance/statement.cpp
        sources/covscript.cpp)

add_library(covscript_debug STATIC
        sources/compiler/codegen.cpp
        sources/compiler/compiler.cpp
        sources/compiler/lexer.cpp
        sources/compiler/parser.cpp
        sources/instance/extension.cpp
        sources/instance/instance.cpp
        sources/instance/runtime.cpp
        sources/instance/statement.cpp
        sources/covscript.cpp)

add_executable(cs sources/standalone.cpp)
add_executable(cs_repl sources/repl.cpp)
add_executable(cs_dbg sources/debugger.cpp)

add_library(test-extension SHARED tests/extension.cpp)
add_executable(test-covscript tests/function_invoker.cpp)

target_compile_definitions(covscript_debug PRIVATE CS_DEBUGGER)

target_link_libraries(cs covscript)
target_link_libraries(cs_repl covscript)
target_link_libraries(cs_dbg covscript_debug)

target_link_libraries(test-extension covscript)
target_link_libraries(test-covscript covscript)

set_target_properties(test-extension PROPERTIES OUTPUT_NAME my_ext)
set_target_properties(test-extension PROPERTIES PREFIX "")
set_target_properties(test-extension PROPERTIES SUFFIX ".cse")

if (UNIX OR APPLE)
    target_link_libraries(cs dl)
    target_link_libraries(cs_repl dl)
    target_link_libraries(cs_dbg dl)
    target_link_libraries(test-covscript dl)
endif ()

install(TARGETS cs cs_repl covscript
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)