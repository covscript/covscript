#!/usr/bin/env bash

set -e
SELF_DIR="$(dirname $(readlink -f $0))"
cd "$SELF_DIR"

PREFIX="/usr"
CXXFLAGS="-std=c++11 -I ../include -fPIE -s -O3 -ldl"
CXX=g++
CP=cp

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
$CXX $CXXFLAGS -DCONFIG_CS_HOME="\"$PREFIX/share/covscript\"" ../sources/standalone.cpp -o ./bin/cs &
$CXX $CXXFLAGS -DCONFIG_CS_HOME="\"$PREFIX/share/covscript\"" ../sources/repl.cpp -o ./bin/cs_repl &
wait

mkdir -p ./$PREFIX/{bin,share}
mkdir -p ./$PREFIX/share/covscript/imports
$CP ./bin/cs ./bin/cs_repl ./$PREFIX/bin
