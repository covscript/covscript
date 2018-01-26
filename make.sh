#!/bin/bash

set -e
SELF_DIR="$(dirname $(readlink -f $0))"
cd "$SELF_DIR"

function set_flag() {
    local append=false

    if [[ "$1" == "--append" ]]; then
        shift
        append=true
    fi

    local var="$1"; shift
    declare -n ref="$var" 
    if [[ "$ref" == "" ]]; then
        ref="$@"
    elif [[ "$append" == true ]]; then
        ref="$ref $@"
    fi
}

DEFAULT_PREFIX="/usr"
DEFAULT_CXXFLAGS="-std=c++11 -I ../include -fPIE -s -O3 -ldl"
DEFAULT_LDFLAGS="-ldl"
DEFAULT_CXX=g++

set_flag CXX $DEFAULT_CXX
set_flag PREFIX $DEFAULT_PREFIX
set_flag --append CXXFLAGS $DEFAULT_CXXFLAGS
set_flag --append LDFLAGS $DEFAULT_LDFLAGS

while [[ "$1"x != ""x ]]; do
    arg="$1"; shift
    case "$arg" in
        --prefix=* ) PREFIX="${arg##--prefix=}" ;;
        --cxxflags=* ) CXXFLAGS="${arg##--cxxflags=}" ;;
        --ldflags=* ) LDFLAGS="${arg##--ldflags=}" ;;
        "--" ) break ;;
    esac
done

mkdir -p build
cd build
mkdir -p bin
$CXX $CXXFLAGS -DCOVSCRIPT_HOME="\"$PREFIX/share/covscript\"" ../sources/standalone.cpp $LDFLAGS -o ./bin/cs &
$CXX $CXXFLAGS -DCOVSCRIPT_HOME="\"$PREFIX/share/covscript\"" ../sources/repl.cpp $LDFLAGS -o ./bin/cs_repl &
wait

