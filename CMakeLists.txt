cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -fPIE -s -O3")
set(CMAKE_LINK_LIBRARY_FLAG "-ldl")
include_directories(include)

if (WIN32)
    execute_process(COMMAND "windres ${CMAKE_CURRENT_SOURCE_DIR}/cs.rc ${CMAKE_CURRENT_SOURCE_DIR}/cs.o")
endif (WIN32)


file(GLOB_RECURSE CS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp")
add_executable(cs ${CS_SRC})
