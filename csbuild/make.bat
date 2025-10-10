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
mkdir csdev\include\covscript
mkdir csdev\lib
xcopy /E /Y include\covscript csdev\include\covscript
xcopy /E /Y csbuild\deps\include\covscript csdev\include\covscript
xcopy /E /Y build\lib         csdev\lib\