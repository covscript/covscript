#!/usr/bin/env bash

set -e
SELF_DIR="$(dirname $(readlink -f $0))"
cd "$SELF_DIR"/..

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
DEFAULT_CXXFLAGS="-std=c++14 -I ../include -s -O3"
DEFAULT_LDFLAGS="-L../lib -lcovscript -ldl"
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

mkdir -p bin
mkdir -p lib
mkdir -p tmp
cd ./tmp
$CXX $CXXFLAGS -DCOVSCRIPT_HOME="\"$PREFIX/share/covscript\"" -fPIC -c ../sources/instance/*.cpp &
$CXX $CXXFLAGS -DCOVSCRIPT_HOME="\"$PREFIX/share/covscript\"" -fPIC -c ../sources/compiler/*.cpp &
$CXX $CXXFLAGS -DCOVSCRIPT_HOME="\"$PREFIX/share/covscript\"" -fPIC -c ../sources/covscript.cpp &
wait
ar -ro ../lib/libcovscript.a *.o
$CXX $CXXFLAGS -DCOVSCRIPT_HOME="\"$PREFIX/share/covscript\"" -fPIE ../sources/standalone.cpp $LDFLAGS -o ../bin/cs &
$CXX $CXXFLAGS -DCOVSCRIPT_HOME="\"$PREFIX/share/covscript\"" -fPIE ../sources/repl.cpp $LDFLAGS -o ../bin/cs_repl &
wait
cd ..
rm -rf ./tmp