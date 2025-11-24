cmake_minimum_required(VERSION 3.16)

project(covscript)

include("${CMAKE_CURRENT_LIST_DIR}/csbuild/csbuild.cmake")

include_directories(include third-party/include third-party/utfcpp)

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
    set(CMAKE_OSX_ARCHITECTURES arm64)
endif ()

add_library(sdk_objects OBJECT ${SOURCE_CODE})

if (ANDROID)
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
        add_subdirectory(third-party)
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
    set(CMAKE_OSX_ARCHITECTURES x86_64)
    set(APPLE_X86_LINK_LIBS Threads::Threads)
    set(APPLE_ARM_LINK_LIBS Threads::Threads)

    # ARM version of covscript
    add_library(sdk_objects_x86 OBJECT ${SOURCE_CODE})
    add_library(covscript_x86 STATIC $<TARGET_OBJECTS:sdk_objects_x86>)
    add_library(covscript_sdk_x86 STATIC $<TARGET_OBJECTS:sdk_objects_x86>)
    add_library(covscript_debug_sdk_x86 STATIC ${SOURCE_CODE})

    # Use libucontext by default in macOS
    if (NOT CS_FIBER_SYSTEM_IMPL)
        add_subdirectory(third-party)
        target_compile_definitions(sdk_objects PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug_sdk PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(sdk_objects_x86 PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_compile_definitions(covscript_debug_sdk_x86 PRIVATE CS_FIBER_LIBUCONTEXT_IMPL)
        target_link_libraries(sdk_objects PUBLIC ucontext)
        target_link_libraries(sdk_objects_x86 PUBLIC ucontext_x86)
        set(APPLE_X86_LINK_LIBS ${APPLE_X86_LINK_LIBS} ucontext)
        set(APPLE_ARM_LINK_LIBS ${APPLE_ARM_LINK_LIBS} ucontext_x86)
    else ()
        message("-- CovScript: Using system implementation of fiber")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif ()

    # Set link options for x86 target
    target_link_libraries(covscript PUBLIC ${APPLE_X86_LINK_LIBS})
    target_link_libraries(covscript_sdk PUBLIC ${APPLE_X86_LINK_LIBS})
    target_link_libraries(covscript_debug_sdk PUBLIC ${APPLE_X86_LINK_LIBS})

    # Set link options for ARM target
    target_link_libraries(covscript_x86 PUBLIC ${APPLE_ARM_LINK_LIBS})
    target_link_libraries(covscript_sdk_x86 PUBLIC ${APPLE_ARM_LINK_LIBS})
    target_link_libraries(covscript_debug_sdk_x86 PUBLIC ${APPLE_ARM_LINK_LIBS})

    set(CMAKE_OSX_ARCHITECTURES arm64)
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
    set(CMAKE_OSX_ARCHITECTURES x86_64)
    add_executable(cs_x86 sources/interpreter.cpp)
    add_executable(cs_dbg_x86 sources/debugger.cpp)
    target_link_libraries(cs_x86 covscript_sdk_x86)
    target_link_libraries(cs_dbg_x86 covscript_debug_sdk_x86)
endif ()

# Tests
if (APPLE AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
    set(CMAKE_OSX_ARCHITECTURES arm64)
endif ()

add_library(test-extension SHARED tests/extension.cpp)
add_library(test-reflection SHARED tests/reflection.cpp)
add_executable(test-covscript tests/function_invoker.cpp)

if (APPLE AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    target_link_libraries(test-extension covscript_x86)
    target_link_libraries(test-reflection covscript_x86)
    target_link_libraries(test-covscript covscript_sdk_x86)
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
