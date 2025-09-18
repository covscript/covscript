#!/usr/bin/env bash
# Rename
mv cs cs_x86
mv cs_dbg cs_dbg_x86
mv libcovscript.a libcovscript_x86.a
mv libcovscript_sdk.a libcovscript_sdk_x86.a
mv libcovscript_debug_sdk.a libcovscript_debug_sdk_x86.a
# Check file information
otool -L libcovscript_x86.a
otool -L libcovscript_sdk_x86.a
otool -L libcovscript_debug_sdk_x86.a
otool -L libcovscript_arm.a
otool -L libcovscript_sdk_arm.a
otool -L libcovscript_debug_sdk_arm.a
# Merge x86_64 and ARM64
lipo -create -output cs cs_x86 cs_arm
lipo -create -output cs_dbg cs_dbg_x86 cs_dbg_arm
lipo -create -output libcovscript.a libcovscript_x86.a libcovscript_arm.a
lipo -create -output libcovscript_sdk.a libcovscript_sdk_x86.a libcovscript_sdk_arm.a
lipo -create -output libcovscript_debug_sdk.a libcovscript_debug_sdk_x86.a libcovscript_debug_sdk_arm.a
rm -f cs_x86 cs_arm cs_dbg_x86 cs_dbg_arm
rm -f libcovscript_x86.a libcovscript_sdk_x86.a libcovscript_debug_sdk_x86.a libcovscript_arm.a libcovscript_sdk_arm.a libcovscript_debug_sdk_arm.a
# Check file information
file cs
file cs_dbg
file libcovscript.a
file libcovscript_sdk.a
file libcovscript_debug_sdk_sdk.a
