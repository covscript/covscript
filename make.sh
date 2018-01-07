#!/bin/bash
if [ "$#" != "1" ]; then
    PREFIX="/usr"
else
    PREFIX=$1
fi
mkdir -p build
cd build
mkdir -p bin
g++ -std=c++11 -I ../include -fPIE -s -O3 -DCS_IMPORT_PATH="$PREFIX/share/covscript/imports" ../sources/standalone.cpp -ldl -o ./bin/cs &
g++ -std=c++11 -I ../include -fPIE -s -O3 -DCS_IMPORT_PATH="$PREFIX/share/covscript/imports" ../sources/repl.cpp -ldl -o ./bin/cs_repl &
wait
