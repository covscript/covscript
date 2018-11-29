@echo off
mkdir bin
mkdir lib
mkdir tmp
cd tmp
set CXX_ARGS=-std=c++11 -I ..\include -static -s -O3
windres -I ..\include ..\sources\win32_rc\standalone.rc cs.o
windres -I ..\include ..\sources\win32_rc\repl.rc cs_repl.o
g++ %CXX_ARGS% -fPIC -c ..\sources\compiler\*.cpp ..\sources\instance\*.cpp ..\sources\*.cpp
ar -ro ..\lib\libcovscript.a *.o
g++ %CXX_ARGS% -fPIE standalone.o cs.o covscript.o -o ..\bin\cs.exe -L..\lib -lcovscript
g++ %CXX_ARGS% -fPIE repl.o cs_repl.o covscript.o -o ..\bin\cs_repl.exe -L..\lib -lcovscript
cd ..
rd /S /Q .\tmp
@echo on