# CovScript Compiler Unit Tests
# Tests critical compiler paths: lexer, parser, trim_expr, opt_expr, translate

set(UNIT_TEST_SOURCES
    main.cpp
    test_lexer.cpp
    test_parser.cpp
    test_trim_expr.cpp
    test_translate.cpp
    test_fiber.cpp
    test_variable.cpp
    test_cni.cpp
    test_regressions.cpp
    test_fixes.cpp
    test_type_ext.cpp
    test_codegen.cpp
    test_system.cpp
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

# Absolute path to the test-extension DLL for the extension tests. Uses the
# config-aware target dir so multi-config generators (MSVC/VS) resolve the same
# directory the POST_BUILD copy below writes to.
target_compile_definitions(cs_unit_tests PRIVATE CS_MY_EXT_PATH="$<TARGET_FILE_DIR:cs_unit_tests>/my_ext.cse")

# Platform-specific link libraries
if (UNIX AND NOT APPLE)
    target_link_libraries(cs_unit_tests pthread dl)
endif ()

if (APPLE)
    target_link_libraries(cs_unit_tests Threads::Threads)
endif ()

# The extension tests load the test-extension DLL (my_ext.cse); copy it next to
# the unit test binary and build it first.
add_dependencies(cs_unit_tests test-extension)
add_custom_command(TARGET cs_unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:test-extension>
            $<TARGET_FILE_DIR:cs_unit_tests>/my_ext.cse)
