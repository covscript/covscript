#!/bin/bash

set -e
SELF_DIR="$(dirname $(readlink -f $0))"
cd "$SELF_DIR"

PREFIX="/usr"
CXXFLAGS="-std=c++11 -I ../include -fPIE -s -O3 -ldl"
LDFLAGS="-ldl"
CXX=g++

while [[ "$1"x != ""x ]]; do
    arg="$1"; shift
    case "$arg" in
        --prefix=* ) PREFIX="${arg##--prefix=}" ;;
        "--" ) break ;;
    esac
done

mkdir -p build
cd build
mkdir -p bin
$CXX $CXXFLAGS -DCOVSCRIPT_HOME="\"$PREFIX/share/covscript\"" ../sources/standalone.cpp $LDFLAGS -o ./bin/cs &
$CXX $CXXFLAGS -DCOVSCRIPT_HOME="\"$PREFIX/share/covscript\"" ../sources/repl.cpp $LDFLAGS -o ./bin/cs_repl &
wait