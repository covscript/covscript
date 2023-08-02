@echo off
cd "%~dp0\.."
set config=MinSizeRel
mkdir cmake-build\msvc_win32
cd    cmake-build\msvc_win32
cmake -G "Visual Studio 16 2019" -A "Win32" ..\..
cmake --build . --config %config% --target cs
cmake --build . --config %config% --target cs_dbg
cd ..\..
rd /S /Q build
mkdir build\bin
mkdir build\lib
xcopy /Y cmake-build\msvc_win32\%config%\*.exe build\bin\
xcopy /Y cmake-build\msvc_win32\%config%\*.lib build\lib\
rd /S /Q csdev
mkdir csdev\include\covscript
mkdir csdev\lib
xcopy /E /Y include\covscript csdev\include\covscript
xcopy /E /Y csbuild\deps\include\covscript csdev\include\covscript
xcopy /E /Y build\lib         csdev\lib\