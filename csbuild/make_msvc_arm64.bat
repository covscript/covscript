@echo off
cd "%~dp0\.."
set config=MinSizeRel
mkdir cmake-build\msvc_arm64
cd    cmake-build\msvc_arm64
cmake -G "Visual Studio 16 2019" -A "ARM64" -DCS_COMPATIBILITY_MODE=1 ..\..
cmake --build . --config %config% --target covscript -- /m
cmake --build . --config %config% --target cs -- /m
cmake --build . --config %config% --target cs_dbg -- /m
cd ..\..
rd /S /Q build
mkdir build\bin
mkdir build\lib
xcopy /Y cmake-build\msvc_arm64\%config%\*.exe build\bin\
xcopy /Y cmake-build\msvc_arm64\%config%\*.lib build\lib\
rd /S /Q csdev
mkdir csdev\include\covscript
mkdir csdev\lib
xcopy /E /Y include\covscript csdev\include\covscript
xcopy /E /Y csbuild\deps\include\covscript csdev\include\covscript
xcopy /E /Y build\lib         csdev\lib\