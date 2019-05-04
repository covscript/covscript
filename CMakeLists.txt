cmake_minimum_required(VERSION 3.4)

project(covscript)
include_directories(include)

# Compiler Options
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /EHsc /utf-8 /w")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-fPIC -s -O3")
else ()
    set(CMAKE_CXX_FLAGS "-fPIC -O3")
endif ()

# Source Code
set(SOURCE_CODE
        sources/compiler/codegen.cpp
        sources/compiler/compiler.cpp
        sources/compiler/lexer.cpp
        sources/compiler/parser.cpp
        sources/instance/extension.cpp
        sources/instance/instance.cpp
        sources/instance/runtime.cpp
        sources/instance/statement.cpp
        sources/covscript.cpp)

# Static Library
add_library(covscript STATIC ${SOURCE_CODE})
add_library(covscript_debug STATIC ${SOURCE_CODE})

target_compile_definitions(covscript_debug PRIVATE CS_DEBUGGER)

# Main Executable
if (MSVC)
    set(rc_flags "/nologo /c65001")
    set(CMAKE_RC_FLAGS ${rc_flags})
endif()

if (WIN32)
    add_executable(cs sources/standalone.cpp sources/win32_rc/standalone.rc)
    add_executable(cs_repl sources/repl.cpp sources/win32_rc/repl.rc)
    add_executable(cs_dbg sources/debugger.cpp sources/win32_rc/debugger.rc)
else()
    add_executable(cs sources/standalone.cpp)
    add_executable(cs_repl sources/repl.cpp)
    add_executable(cs_dbg sources/debugger.cpp)
endif ()

target_link_libraries(cs covscript)
target_link_libraries(cs_repl covscript)
target_link_libraries(cs_dbg covscript_debug)

# Tests
add_library(test-extension SHARED tests/extension.cpp)
add_executable(test-covscript tests/function_invoker.cpp)

target_link_libraries(test-extension covscript)
target_link_libraries(test-covscript covscript)

set_target_properties(test-extension PROPERTIES OUTPUT_NAME my_ext)
set_target_properties(test-extension PROPERTIES PREFIX "")
set_target_properties(test-extension PROPERTIES SUFFIX ".cse")

# Link libdl in UNIX and APPLE platform
if (UNIX OR APPLE)
    target_link_libraries(cs dl)
    target_link_libraries(cs_dbg dl)
    target_link_libraries(cs_repl dl)
    target_link_libraries(test-covscript dl)
endif ()