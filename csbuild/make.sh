#!/usr/bin/env bash
CPU_NUM=8
if command -v getconf >/dev/null 2>&1; then
    detected=$(getconf _NPROCESSORS_ONLN 2>/dev/null)
    if [[ "$detected" =~ ^[0-9]+$ ]] && [[ "$detected" -gt 0 ]]; then
        CPU_NUM=$detected
        echo  -- Setting parallel jobs to $detected automatically
    fi
fi
CURRENT_FOLDER=$(dirname $(readlink -f "$0"))
cd $CURRENT_FOLDER/..
mkdir -p cmake-build/unix
cd       cmake-build/unix
cmake -G "Unix Makefiles" ../..
make cs cs_dbg covscript -j$CPU_NUM
# Running lipo in macOS
if [[ "$(uname)" == "Darwin" ]]; then
    make cs_x86 cs_dbg_x86 covscript_x86 -j$CPU_NUM
    bash ../../csbuild/create_universal.sh
fi
cd ../..
rm -rf build
mkdir -p build/bin
mkdir -p build/lib
find cmake-build/unix -maxdepth 1 -type f -name 'cs*' -exec mv {} build/bin/ \;
find cmake-build/unix -maxdepth 1 -type f -name 'libcovscript*.a' -exec mv {} build/lib/ \;
strip -x build/bin/cs*
rm -rf csdev
mkdir -p csdev/include/covscript
mkdir -p csdev/lib
cp -r include/covscript csdev/include/
cp -r third-party/include/ csdev/include/
cp -r build/lib         csdev/
