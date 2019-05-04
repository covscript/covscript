@echo off
set config=MinSizeRel
mkdir cmake-build\msvc
cd    cmake-build\msvc
cmake -G "Visual Studio 16 2019" ..\..
cmake --build . --config %config% --target covscript
cmake --build . --config %config% --target covscript_debug
cmake --build . --config %config% --target cs
cmake --build . --config %config% --target cs_repl
cmake --build . --config %config% --target cs_dbg
cd ..\..
rd /S /Q build
mkdir build\bin
mkdir build\lib
xcopy /Y cmake-build\msvc\%config%\*.exe build\bin\
xcopy /Y cmake-build\msvc\%config%\*.lib build\lib\