#!/usr/bin/env bash
# Rename
mv cs cs_x86
mv cs_dbg cs_dbg_x86
# Merge libcovscript and libucontext
libtool -static -o libcovscript_x86_merge.a libcovscript.a libucontext_x86.a
libtool -static -o libcovscript_debug_x86_merge.a libcovscript_debug.a libucontext_x86.a
libtool -static -o libcovscript_arm_merge.a libcovscript_arm.a libucontext_arm.a
libtool -static -o libcovscript_debug_arm_merge.a libcovscript_debug_arm.a libucontext_arm.a
rm -f libcovscript.a libcovscript_arm.a libcovscript_debug.a libcovscript_debug_arm.a
rm -f libucontext_arm.a libucontext_x86.a
# Check file information
otool -L libcovscript_x86_merge.a
otool -L libcovscript_debug_x86_merge.a
otool -L libcovscript_arm_merge.a
otool -L libcovscript_debug_arm_merge.a
# Merge x86_64 and ARM64
lipo -create -output cs cs_x86 cs_arm
lipo -create -output cs_dbg cs_dbg_x86 cs_dbg_arm
lipo -create -output libcovscript.a libcovscript_x86_merge.a libcovscript_arm_merge.a
lipo -create -output libcovscript_debug.a libcovscript_debug_x86_merge.a libcovscript_debug_arm_merge.a
rm -f cs_x86 cs_arm cs_dbg_x86 cs_dbg_arm
rm -f libcovscript_x86_merge.a libcovscript_debug_x86_merge.a libcovscript_arm_merge.a libcovscript_debug_arm_merge.a
# Check file information
file cs
file cs_dbg
file libcovscript.a
file libcovscript_debug.a
