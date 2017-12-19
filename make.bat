@mkdir build
@cd build
@mkdir bin
@windres ..\cs.rc cs.o
@g++ -std=c++11 -I ..\include -static -fPIE -s -O3 ..\sources\standalone.cpp cs.o -o .\bin\cs.exe
@g++ -std=c++11 -I ..\include -static -fPIE -s -O3 ..\sources\repl.cpp cs.o -o .\bin\cs_repl.exe
@del /F /Q cs.o
