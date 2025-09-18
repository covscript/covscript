cmake_minimum_required(VERSION 3.13)

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
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
endif ()

add_library(covscript STATIC csbuild/dummy.cpp)
add_library(covscript_sdk STATIC ${SOURCE_CODE})
add_library(covscript_debug_sdk STATIC ${SOURCE_CODE})

target_compile_definitions(covscript_debug_sdk PRIVATE CS_DEBUGGER)

# Link extra libraries in Linux
if (UNIX AND NOT APPLE)
    target_link_libraries(covscript_sdk pthread dl)
    target_link_libraries(covscript_debug_sdk pthread dl)

    include(CheckCSourceCompiles)

    check_c_source_compiles("
        #include <features.h>
        #ifndef __GLIBC__
        #error Not glibc
        #endif
        int main() { return 0; }
    " HAVE_GLIBC)

    # Use system implementation by default in Linux
    if (NOT HAVE_GLIBC OR CS_FIBER_LIBUCONTEXT_IMPL)
        message("-- CovScript: Using libucontext implementation of fiber")
        add_subdirectory(csbuild/deps)
        target_compile_definitions(covscript_sdk PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug_sdk PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_link_libraries(covscript_sdk ucontext)
        target_link_libraries(covscript_debug_sdk ucontext)
    endif ()
endif ()

# Process universal binary in macOS
if (APPLE)
    # ARM version of covscript
    set(CMAKE_OSX_ARCHITECTURES arm64)
    add_library(covscript_arm STATIC csbuild/dummy.cpp)
    add_library(covscript_sdk_arm STATIC ${SOURCE_CODE})
    add_library(covscript_debug_sdk_arm STATIC ${SOURCE_CODE})

    # Set link options for x86 target
    target_link_libraries(covscript_sdk pthread dl)
    target_link_libraries(covscript_debug_sdk pthread dl)

    # Set link options for ARM target
    target_link_libraries(covscript_sdk_arm pthread dl)
    target_link_libraries(covscript_debug_sdk_arm pthread dl)

    # Use libucontext by default in macOS
    if (NOT CS_FIBER_SYSTEM_IMPL)
        add_subdirectory(csbuild/deps)
        target_compile_definitions(covscript_sdk PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug_sdk PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_sdk_arm PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug_sdk_arm PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_link_libraries(covscript_sdk ucontext)
        target_link_libraries(covscript_debug_sdk ucontext)
        target_link_libraries(covscript_sdk_arm ucontext_arm)
        target_link_libraries(covscript_debug_sdk_arm ucontext_arm)
    else ()
        message("-- CovScript: Using system implementation of fiber")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif ()
endif ()

# Main Executable
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
endif ()

if (WIN32)
    add_executable(cs sources/interpreter.cpp sources/win32_rc/interpreter.rc)
    add_executable(cs_dbg sources/debugger.cpp sources/win32_rc/debugger.rc)
else ()
    add_executable(cs sources/interpreter.cpp)
    add_executable(cs_dbg sources/debugger.cpp)
endif ()

if (ANDROID)
    target_link_options(cs PRIVATE "-Wl,--export-dynamic")
    target_link_options(cs_dbg PRIVATE "-Wl,--export-dynamic")
else ()
    target_link_libraries(covscript covscript_sdk)
endif ()

target_link_libraries(cs covscript_sdk)
target_link_libraries(cs_dbg covscript_debug_sdk)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES arm64)
    target_link_libraries(covscript_arm covscript_sdk_arm)
    add_executable(cs_arm sources/interpreter.cpp)
    add_executable(cs_dbg_arm sources/debugger.cpp)
    target_link_libraries(cs_arm covscript_sdk_arm)
    target_link_libraries(cs_dbg_arm covscript_debug_sdk_arm)
endif ()

# Tests
if (APPLE AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set(CMAKE_OSX_ARCHITECTURES x86_64)
endif ()

add_library(test-extension SHARED tests/extension.cpp)
add_library(test-reflection SHARED tests/reflection.cpp)
add_executable(test-covscript tests/function_invoker.cpp)

if (APPLE AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
    target_link_libraries(test-extension covscript_arm)
    target_link_libraries(test-reflection covscript_arm)
    target_link_libraries(test-covscript covscript_sdk_arm)
else ()
    target_link_libraries(test-extension covscript)
    target_link_libraries(test-reflection covscript)
    target_link_libraries(test-covscript covscript_sdk)
endif ()

set_target_properties(test-extension PROPERTIES OUTPUT_NAME my_ext)
set_target_properties(test-extension PROPERTIES PREFIX "")
set_target_properties(test-extension PROPERTIES SUFFIX ".cse")

set_target_properties(test-reflection PROPERTIES OUTPUT_NAME reflect)
set_target_properties(test-reflection PROPERTIES PREFIX "")
set_target_properties(test-reflection PROPERTIES SUFFIX ".cse")
