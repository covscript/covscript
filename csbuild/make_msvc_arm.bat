@echo off
set config=MinSizeRel
mkdir cmake-build\msvc_arm
cd    cmake-build\msvc_arm
set CS_COMPATIBILITY_MODE=ENABLE
cmake -G "Visual Studio 15 2017 ARM" ..\..
cmake --build . --config %config% --target covscript
cmake --build . --config %config% --target covscript_debug
cmake --build . --config %config% --target cs
cmake --build . --config %config% --target cs_repl
cmake --build . --config %config% --target cs_dbg
cd ..\..
rd /S /Q build
mkdir build\bin
mkdir build\lib
xcopy /Y cmake-build\msvc_arm\%config%\*.exe build\bin\
xcopy /Y cmake-build\msvc_arm\%config%\*.lib build\lib\
rd /S /Q csdev
mkdir csdev\include\covscript
mkdir csdev\lib
xcopy /E /Y include\covscript csdev\include\covscript
xcopy /E /Y build\lib         csdev\lib\