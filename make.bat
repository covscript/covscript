@mkdir build
@cd build
@mkdir bin
@mkdir lib
@windres -I ..\include ..\sources\standalone.rc cs.o
@windres -I ..\include ..\sources\repl.rc cs_repl.o
@g++ -std=c++11 -I ..\include -shared -static -fPIC -s -O3 ..\sources\covscript.cpp -o .\lib\libcovscript.dll
@g++ -std=c++11 -I ..\include -static -fPIE -s -O3 ..\sources\standalone.cpp cs.o -o .\bin\cs.exe
@g++ -std=c++11 -I ..\include -static -fPIE -s -O3 ..\sources\repl.cpp cs_repl.o -o .\bin\cs_repl.exe
@del /F /Q *.o