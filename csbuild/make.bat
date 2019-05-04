@echo off
mkdir cmake-build\mingw-w64
cd    cmake-build\mingw-w64
cmake -G "MinGW Makefiles" ..\..
cmake --build . --target covscript        -- -j4
cmake --build . --target covscript_debug  -- -j4
cmake --build . --target cs               -- -j4
cmake --build . --target cs_repl          -- -j4
cmake --build . --target cs_dbg           -- -j4
cd ..\..
rd /S /Q build
mkdir build\bin
mkdir build\lib
xcopy /Y cmake-build\mingw-w64\*.exe build\bin\
xcopy /Y cmake-build\mingw-w64\*.a   build\lib\