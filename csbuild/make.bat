@echo off
cd "%~dp0\.."
mkdir cmake-build\mingw-w64
cd    cmake-build\mingw-w64
cmake -G "MinGW Makefiles" ..\..
cmake --build . --target cs cs_dbg covscript -- -j8
cd ..\..
rd /S /Q build
mkdir build\bin
mkdir build\lib
xcopy /Y cmake-build\mingw-w64\*.exe build\bin\
xcopy /Y cmake-build\mingw-w64\*.a   build\lib\
strip -x build\bin\cs*
rd /S /Q csdev
mkdir csdev\include
mkdir csdev\lib
xcopy /E /Y include\ csdev\include\
xcopy /E /Y third-party\include\ csdev\include\
xcopy /E /Y build\lib            csdev\lib\
xcopy /Y csbuild\csbuild.cmake   csdev\