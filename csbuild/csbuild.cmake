# Profiling
option(CS_ENABLE_PROFILING "Enable profiling support" OFF)

# Align SVO on different cache line
option(CS_VAR_SVO_ALIGN "Custom SVO alignment (set > 0 to enable)" 0)

# Force use heap allocator
option(CS_DISABLE_VAR_SVO "Force heap allocator" OFF)

# Aggressive optimization (force inline, etc.)
option(CS_AGGRESSIVE_OPTIMIZE "Enable aggressive optimizations" OFF)

# Use STL containers instead of custom high-performance
option(CS_COMPATIBILITY_MODE "Enable compatible mode" OFF)

# Compiler Options
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    # MSVC Windows
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2 /utf-8 /DNDEBUG /MP /w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /EHsc /utf-8 /DNDEBUG /Zc:__cplusplus /MP /w")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} /nologo /c65001")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Common flags for GCC/Clang
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -DNDEBUG -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG -fPIC -Wno-deprecated-declarations")
    if (WIN32)
        # MinGW or Clang on Windows
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --static")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --static")
    elseif (UNIX AND NOT APPLE)
        # Linux
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto=8 -fno-plt")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=8 -fno-plt")
        else ()
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
        endif ()
    elseif (APPLE)
        # macOS
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
        set(CS_AGGRESSIVE_OPTIMIZE ON CACHE BOOL "Enable aggressive optimizations" FORCE)
    endif ()
endif ()

if(CS_ENABLE_PROFILING)
    message("-- CovScript: Enable Profiling Support")
    add_compile_definitions(CS_ENABLE_PROFILING)
endif()

if(CS_VAR_SVO_ALIGN GREATER 0)
    message("-- CovScript: Custom SVO Alignment = ${CS_VAR_SVO_ALIGN}")
    add_compile_definitions(CS_VAR_SVO_ALIGN=${CS_VAR_SVO_ALIGN})
endif()

if(CS_DISABLE_VAR_SVO)
    message("-- CovScript: Force Heap Allocator")
    add_compile_definitions(CS_DISABLE_VAR_SVO)
endif()

if(CS_AGGRESSIVE_OPTIMIZE)
    message("-- CovScript: Enable Aggressive Optimizations")
    add_compile_definitions(CS_AGGRESSIVE_OPTIMIZE)
endif()

if (CS_COMPATIBILITY_MODE)
    message("-- CovScript: Configuring Compatibility Mode")
    add_compile_definitions(CS_COMPATIBILITY_MODE)
endif ()

if (ANDROID)
    if(DEFINED ENV{PREFIX})
        set(PREFIX_DIR "$ENV{PREFIX}")
        if(IS_DIRECTORY "${PREFIX_DIR}")
            string(FIND "${PREFIX_DIR}" "com.termux" TERMUX_POS)
            if(NOT TERMUX_POS EQUAL -1)
                message("-- CovScript: Termux environment detected")
                message("-- CovScript: Setting COVSCRIPT_PLATFORM_HOME to ${PREFIX_DIR}/share/covscript")
                add_compile_definitions(COVSCRIPT_PLATFORM_HOME=\"${PREFIX_DIR}/share/covscript\")
            endif ()
        endif ()
    endif ()
    message("-- CovScript: Android NDK environment detected. Patch for extensions applied")
endif ()