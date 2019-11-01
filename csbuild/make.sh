#!/usr/bin/env bash
mkdir -p cmake-build/unix
cd       cmake-build/unix
cmake -G "Unix Makefiles" ../..
cmake --build . --target covscript        -- -j4
cmake --build . --target covscript_debug  -- -j4
cmake --build . --target cs               -- -j4
cmake --build . --target cs_dbg           -- -j4
cd ../..
rm -rf build
mkdir -p build/bin
mkdir -p build/lib
cp cmake-build/unix/cs* build/bin/
cp cmake-build/unix/*.a build/lib/
rm -rf csdev
mkdir -p csdev/include/covscript
mkdir -p csdev/lib
cp -r include/covscript csdev/include/
cp -r build/lib         csdev/