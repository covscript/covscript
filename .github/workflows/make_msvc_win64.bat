@echo off
set config=MinSizeRel
mkdir cmake-build\msvc_win64
cd    cmake-build\msvc_win64
cmake -G "Visual Studio 17 2022" -A x64 ..\..
cmake --build . --config %config% --target covscript
cmake --build . --config %config% --target covscript_debug
cmake --build . --config %config% --target cs
cmake --build . --config %config% --target cs_dbg
cd ..\..
rd /S /Q build
mkdir build\bin
mkdir build\lib
xcopy /Y cmake-build\msvc_win64\%config%\*.exe build\bin\
xcopy /Y cmake-build\msvc_win64\%config%\*.lib build\lib\
rd /S /Q csdev
mkdir csdev\include\covscript
mkdir csdev\lib
xcopy /E /Y include\covscript csdev\include\covscript
xcopy /E /Y build\lib         csdev\lib\
