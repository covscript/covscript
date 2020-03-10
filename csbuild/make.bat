@echo off
cd "%~dp0\.."
mkdir cmake-build\mingw-w64
cd    cmake-build\mingw-w64
cmake -G "MinGW Makefiles" ..\..
cmake --build . --target covscript        -- -j4
cmake --build . --target covscript_debug  -- -j4
cmake --build . --target cs               -- -j4
cmake --build . --target cs_dbg           -- -j4
cd ..\..
rd /S /Q build
mkdir build\bin
mkdir build\lib
xcopy /Y cmake-build\mingw-w64\*.exe build\bin\
xcopy /Y cmake-build\mingw-w64\*.a   build\lib\
rd /S /Q csdev
mkdir csdev\include\covscript
mkdir csdev\lib
xcopy /E /Y include\covscript csdev\include\covscript
xcopy /E /Y build\lib         csdev\lib\