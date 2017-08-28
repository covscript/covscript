cmake_minimum_required(VERSION 3.4)
project(covscript)
include_directories(include)

set(SOURCE_FILES
        sources/extensions/array_extension.hpp
        sources/extensions/char_extension.hpp
        sources/extensions/darwin_extension.hpp
        sources/extensions/except_extension.hpp
        sources/extensions/hash_map_extension.hpp
        sources/extensions/iostream_extension.hpp
        sources/extensions/list_extension.hpp
        sources/extensions/math_extension.hpp
        sources/extensions/pair_extension.hpp
        sources/extensions/runtime_extension.hpp
        sources/extensions/string_extension.hpp
        sources/extensions/system_extension.hpp
        sources/any.hpp
        sources/arglist.hpp
        sources/cni.hpp
        sources/core.hpp
        sources/covscript.cpp
        sources/covscript.hpp
        sources/exceptions.hpp
        sources/extension.hpp
        sources/grammar.hpp
        sources/lexer.hpp
        sources/optimize.hpp
        sources/parser.hpp
        sources/runtime.hpp
        sources/symbols.hpp
        sources/typedef.hpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -fPIE -s -O3")
if(UNIX OR APPLE)
    set(CMAKE_LINK_LIBRARY_FLAG "-ldl")
endif()

add_executable(cs ${SOURCE_FILES})
add_executable(test-config tests/config.cpp)
add_library(test-extension SHARED tests/extension.cpp)