# CovScript Compiler Unit Tests
# Tests critical compiler paths: lexer, parser, trim_expr, opt_expr, translate

set(UNIT_TEST_SOURCES
    main.cpp
    test_lexer.cpp
    test_parser.cpp
    test_trim_expr.cpp
    test_translate.cpp
)

add_executable(cs_unit_tests ${UNIT_TEST_SOURCES})

# Link against the SDK (includes compiler + runtime)
if (APPLE AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    target_link_libraries(cs_unit_tests covscript_sdk_x86)
else ()
    target_link_libraries(cs_unit_tests covscript_sdk)
endif ()

# Enable access to compiler private APIs for unit tests
target_compile_definitions(cs_unit_tests PRIVATE CS_UNIT_TEST)

# Platform-specific link libraries
if (UNIX AND NOT APPLE)
    target_link_libraries(cs_unit_tests pthread dl)
endif ()

if (APPLE)
    target_link_libraries(cs_unit_tests Threads::Threads)
endif ()
