@echo off
set CPU_NUM=8
if defined NUMBER_OF_PROCESSORS (
    set /A detected=%NUMBER_OF_PROCESSORS%
    if %detected% GTR 0 (
        set CPU_NUM=%detected%
        echo  -- Setting parallel jobs to %CPU_NUM% automatically
    )
)
cd "%~dp0\.."
mkdir cmake-build\mingw-w64
cd    cmake-build\mingw-w64
cmake -G "MinGW Makefiles" ..\..
cmake --build . --target cs               -- -j8
cmake --build . --target cs_dbg           -- -j8
cmake --build . --target covscript        -- -j8
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
xcopy /E /Y csbuild\deps\include\covscript csdev\include\covscript
xcopy /E /Y build\lib         csdev\lib\