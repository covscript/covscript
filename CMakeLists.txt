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
    find_package(Threads REQUIRED)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
endif ()

add_library(sdk_objects OBJECT ${SOURCE_CODE})

if (ANDROID)
    if(DEFINED ENV{PREFIX})
        set(PREFIX_DIR "$ENV{PREFIX}")
        if(IS_DIRECTORY "${PREFIX_DIR}")
            string(FIND "${PREFIX_DIR}" "com.termux" TERMUX_POS)
            if(NOT TERMUX_POS EQUAL -1)
                message("-- CovScript: Termux environment detected")
                message("-- CovScript: Setting COVSCRIPT_PLATFORM_HOME to ${PREFIX_DIR}/share/covscript")
                add_compile_definitions(COVSCRIPT_PLATFORM_HOME=\"${PREFIX_DIR}/share/covscript\")
            endif()
        endif()
    endif()
    message("-- CovScript: Android NDK environment detected. Patch for extensions applied")
    add_library(covscript STATIC csbuild/dummy.cpp)
else ()
    add_library(covscript STATIC $<TARGET_OBJECTS:sdk_objects>)
endif ()

add_library(covscript_sdk STATIC $<TARGET_OBJECTS:sdk_objects>)
add_library(covscript_debug_sdk STATIC ${SOURCE_CODE})

target_compile_definitions(covscript_debug_sdk PRIVATE CS_DEBUGGER)

# Link extra libraries in Linux
if (UNIX AND NOT APPLE)
    set(UNIX_LINK_LIBS pthread dl)
    target_link_libraries(covscript PUBLIC ${UNIX_LINK_LIBS})
    target_link_libraries(covscript_sdk PUBLIC ${UNIX_LINK_LIBS})
    target_link_libraries(covscript_debug_sdk PUBLIC ${UNIX_LINK_LIBS})

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
        target_compile_definitions(sdk_objects PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug_sdk PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_link_libraries(sdk_objects PUBLIC ucontext)
        target_link_libraries(covscript PUBLIC ucontext)
        target_link_libraries(covscript_sdk PUBLIC ucontext)
        target_link_libraries(covscript_debug_sdk PUBLIC ucontext)
    endif ()
endif ()

# Process universal binary in macOS
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES arm64)
    set(APPLE_X86_LINK_LIBS Threads::Threads)
    set(APPLE_ARM_LINK_LIBS Threads::Threads)

    # ARM version of covscript
    add_library(sdk_objects_arm OBJECT ${SOURCE_CODE})
    add_library(covscript_arm STATIC $<TARGET_OBJECTS:sdk_objects_arm>)
    add_library(covscript_sdk_arm STATIC $<TARGET_OBJECTS:sdk_objects_arm>)
    add_library(covscript_debug_sdk_arm STATIC ${SOURCE_CODE})

    # Use libucontext by default in macOS
    if (NOT CS_FIBER_SYSTEM_IMPL)
        add_subdirectory(csbuild/deps)
        target_compile_definitions(sdk_objects PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug_sdk PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(sdk_objects_arm PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug_sdk_arm PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_link_libraries(sdk_objects PUBLIC ucontext)
        target_link_libraries(sdk_objects_arm PUBLIC ucontext_arm)
        set(APPLE_X86_LINK_LIBS ${APPLE_X86_LINK_LIBS} ucontext)
        set(APPLE_ARM_LINK_LIBS ${APPLE_ARM_LINK_LIBS} ucontext_arm)
    else ()
        message("-- CovScript: Using system implementation of fiber")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif ()

    # Set link options for x86 target
    target_link_libraries(covscript PUBLIC ${APPLE_X86_LINK_LIBS})
    target_link_libraries(covscript_sdk PUBLIC ${APPLE_X86_LINK_LIBS})
    target_link_libraries(covscript_debug_sdk PUBLIC ${APPLE_X86_LINK_LIBS})

    # Set link options for ARM target
    target_link_libraries(covscript_arm PUBLIC ${APPLE_ARM_LINK_LIBS})
    target_link_libraries(covscript_sdk_arm PUBLIC ${APPLE_ARM_LINK_LIBS})
    target_link_libraries(covscript_debug_sdk_arm PUBLIC ${APPLE_ARM_LINK_LIBS})

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
endif ()

target_link_libraries(cs covscript_sdk)
target_link_libraries(cs_dbg covscript_debug_sdk)

if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES arm64)
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
