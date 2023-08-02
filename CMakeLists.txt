cmake_minimum_required(VERSION 3.4)

project(covscript)

# Compiler Options
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /EHsc /utf-8 /w")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(rc_flags "/nologo /c65001")
    set(CMAKE_RC_FLAGS ${rc_flags})
elseif (CMAKE_COMPILER_IS_GNUCXX)
    if (WIN32)
        set(CMAKE_CXX_FLAGS "--static -fPIC -s -O3")
    else ()
        set(CMAKE_CXX_FLAGS "-fPIC -s -O3")
    endif ()
else ()
    set(CMAKE_CXX_FLAGS "-fPIC -O3")
endif ()

if (CS_COMPATIBILITY_MODE)
    message("CovScript: Configuring Compatibility Mode")
    add_compile_definitions(CS_COMPATIBILITY_MODE)
endif ()

include_directories(include csbuild/deps/include)

# Source Code
set(SOURCE_CODE
        sources/compiler/codegen.cpp
        sources/compiler/compiler.cpp
        sources/compiler/lexer.cpp
        sources/compiler/parser.cpp
        sources/instance/type_ext.cpp
        sources/instance/instance.cpp
        sources/instance/runtime.cpp
        sources/instance/statement.cpp
        sources/system/common.cpp
        sources/covscript.cpp)

# Static Library
add_library(covscript STATIC ${SOURCE_CODE})
add_library(covscript_debug STATIC ${SOURCE_CODE})

target_compile_definitions(covscript_debug PRIVATE CS_DEBUGGER)

# Link extra libraries in Linux
if (LINUX)
    target_link_libraries(covscript pthread dl)
    target_link_libraries(covscript_debug pthread dl)

    # Use system implementation by default in Linux
    if (CS_FIBER_LIBUCONTEXT_IMPL)
        message("-- CovScript: Using libucontext implementation of fiber")
        add_subdirectory(csbuild/deps)
        target_compile_definitions(covscript PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_link_libraries(covscript ucontext)
        target_link_libraries(covscript_debug ucontext)
    endif ()
endif ()

# Process universal binary in macOS
if (APPLE)
    # ARM version of covscript
    add_library(covscript_arm STATIC ${SOURCE_CODE})
    add_library(covscript_debug_arm STATIC ${SOURCE_CODE})

    # Set link options for x86 target
    target_link_libraries(covscript pthread dl)
    target_link_libraries(covscript_debug pthread dl)

    # Set link options for ARM target
    target_link_libraries(covscript_arm pthread dl)
    target_link_libraries(covscript_debug_arm pthread dl)

    # Use libucontext by default in macOS
    if (NOT CS_FIBER_SYSTEM_IMPL)
        add_subdirectory(csbuild/deps)
        target_compile_definitions(covscript PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_arm PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug_arm PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_link_libraries(covscript ucontext)
        target_link_libraries(covscript_debug ucontext)
        target_link_libraries(covscript_arm ucontext_arm)
        target_link_libraries(covscript_debug_arm ucontext_arm)
    else ()
        message("-- CovScript: Using system implementation of fiber")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif ()
endif ()

# Main Executable
if (WIN32)
    add_executable(cs sources/interpreter.cpp sources/win32_rc/interpreter.rc)
    add_executable(cs_dbg sources/debugger.cpp sources/win32_rc/debugger.rc)
else ()
    add_executable(cs sources/interpreter.cpp)
    add_executable(cs_dbg sources/debugger.cpp)
endif ()

target_link_libraries(cs covscript)
target_link_libraries(cs_dbg covscript_debug)

if (APPLE)
    add_executable(cs_arm sources/interpreter.cpp)
    add_executable(cs_dbg_arm sources/debugger.cpp)
    target_link_libraries(cs_arm covscript_arm)
    target_link_libraries(cs_dbg_arm covscript_debug_arm)
endif ()

# Tests
add_library(test-extension SHARED tests/extension.cpp)
add_library(test-reflection SHARED tests/reflection.cpp)
add_executable(test-covscript tests/function_invoker.cpp)

target_link_libraries(test-extension covscript)
target_link_libraries(test-reflection covscript)
target_link_libraries(test-covscript covscript)

set_target_properties(test-extension PROPERTIES OUTPUT_NAME my_ext)
set_target_properties(test-extension PROPERTIES PREFIX "")
set_target_properties(test-extension PROPERTIES SUFFIX ".cse")

set_target_properties(test-reflection PROPERTIES OUTPUT_NAME reflect)
set_target_properties(test-reflection PROPERTIES PREFIX "")
set_target_properties(test-reflection PROPERTIES SUFFIX ".cse")
