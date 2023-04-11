cmake_minimum_required(VERSION 3.4)

project(covscript)
include_directories(include)

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

if (DEFINED ENV{CS_COMPATIBILITY_MODE})
    add_compile_definitions(CS_COMPATIBILITY_MODE)
endif ()

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

# Link libdl in UNIX platform
if (UNIX)
    target_link_libraries(covscript pthread dl)
    target_link_libraries(covscript_debug pthread dl)
endif ()

# Process universal binary in APPLE platform
if (APPLE)
    enable_language(C ASM)
    include_directories(csbuild/libucontext/include)

    # ARM version of covscript
    add_library(covscript_arm STATIC ${SOURCE_CODE})
    add_library(covscript_debug_arm STATIC ${SOURCE_CODE})

    # x86 version of ucontext
    add_library(ucontext_x86 STATIC
            csbuild/libucontext/arch/x86_64/getcontext.S
            csbuild/libucontext/arch/x86_64/makecontext.c
            csbuild/libucontext/arch/x86_64/setcontext.S
            csbuild/libucontext/arch/x86_64/swapcontext.S
            csbuild/libucontext/arch/x86_64/trampoline.c)
    target_compile_options(ucontext_x86 PUBLIC -target x86_64-apple-macos10.12)
    target_link_options(ucontext_x86 PUBLIC -target x86_64-apple-macos10.12)
    target_include_directories(ucontext_x86 PRIVATE csbuild/libucontext/arch/common)
    target_include_directories(ucontext_x86 PUBLIC csbuild/libucontext/arch/x86_64/include)

    # ARM version of ucontext
    add_library(ucontext_arm STATIC
            csbuild/libucontext/arch/aarch64/getcontext.S
            csbuild/libucontext/arch/aarch64/makecontext.c
            csbuild/libucontext/arch/aarch64/setcontext.S
            csbuild/libucontext/arch/aarch64/swapcontext.S
            csbuild/libucontext/arch/aarch64/trampoline.c)
    target_compile_options(ucontext_arm PUBLIC -target arm64-apple-macos11)
    target_link_options(ucontext_arm PUBLIC -target arm64-apple-macos11)
    target_include_directories(ucontext_arm PRIVATE csbuild/libucontext/arch/common)
    target_include_directories(ucontext_arm PUBLIC csbuild/libucontext/arch/aarch64/include)

    # Set link options for x86 target
    target_link_libraries(covscript pthread dl ucontext_x86)
    target_link_libraries(covscript_debug pthread dl ucontext_x86)

    # Set link options for ARM target
    target_link_libraries(covscript_arm pthread dl ucontext_arm)
    target_link_libraries(covscript_debug_arm pthread dl ucontext_arm)
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
