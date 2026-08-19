#!/usr/bin/env bash
function format() {
    find "$1" -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' \) -exec clang-format -i {} +
}
CURRENT_FOLDER=$(dirname $(readlink -f "$0"))
cd $CURRENT_FOLDER/..
format ./include
format ./sources
format ./unit_tests
format ./tests
