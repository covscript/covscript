@mkdir build
@cd build
@mkdir bin
@windres ..\cs.rc cs.o
@windres ..\cs_repl.rc cs_repl.o
@g++ -std=c++11 -I ..\include -static -fPIE -s -O3 ..\sources\standalone.cpp cs.o -o .\bin\cs.exe
@g++ -std=c++11 -I ..\include -static -fPIE -s -O3 ..\sources\repl.cpp cs_repl.o -o .\bin\cs_repl.exe
@del /F /Q *.o