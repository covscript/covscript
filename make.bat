@windres .\cs.rc cs.o
@g++ -std=c++11 -static -fPIE -s -O3 .\sources\covscript.cpp cs.o -o cs.exe
@del /F /Q cs.o
