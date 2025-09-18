#!/usr/bin/env bash
mkdir -p cmake-build/unix
cd       cmake-build/unix
cmake -G "Unix Makefiles" ../..
cmake --build . --target cs               -- -j8
cmake --build . --target cs_dbg           -- -j8
cmake --build . --target covscript        -- -j8
# Running lipo in macOS
if [[ "$(uname)" == "Darwin" ]]; then
    cmake --build . --target cs_arm       -- -j8
    cmake --build . --target cs_dbg_arm   -- -j8
    cmake --build . --target covscript    -- -j8
    bash ../../csbuild/create_universal.sh
fi
cd ../..
rm -rf build
mkdir -p build/bin
mkdir -p build/lib
mv cmake-build/unix/cs* build/bin/
mv cmake-build/unix/*.a build/lib/
rm -rf csdev
mkdir -p csdev/include/covscript
mkdir -p csdev/lib
cp -r include/covscript csdev/include/
cp -r csbuild/deps/include/covscript csdev/include/
cp -r build/lib         csdev/
