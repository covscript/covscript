#!/usr/bin/env bash
mkdir -p cmake-build/unix
cd       cmake-build/unix
cmake -G "Unix Makefiles" ../..
cmake --build . --target cs               -- -j8
cmake --build . --target cs_dbg           -- -j8
# Running lipo in macOS
if [[ "$(uname)" == "Darwin" ]]; then
    cmake --build . --target cs_arm       -- -j8
    cmake --build . --target cs_dbg_arm   -- -j8
    lipo -create -output cs_univ cs cs_arm
    lipo -create -output cs_dbg_univ cs_dbg cs_dbg_arm
    lipo -create -output libcovscript_univ.a libcovscript.a libcovscript_arm.a
    lipo -create -output libcovscript_debug_univ.a libcovscript_debug.a libcovscript_debug_arm.a
    rm cs cs_arm cs_dbg cs_dbg_arm libcovscript.a libcovscript_arm.a libcovscript_debug.a libcovscript_debug_arm.a libucontext_arm.a libucontext_x86.a
    mv cs_univ cs
    file cs
    mv cs_dbg_univ cs_dbg
    file cs_dbg
    mv libcovscript_univ.a libcovscript.a
    file libcovscript.a
    mv libcovscript_debug_univ.a libcovscript_debug.a
    file libcovscript_debug.a
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
cp -r build/lib         csdev/
