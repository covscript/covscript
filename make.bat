@windres .\cs.rc cs.o
@g++ -std=c++11 -static -fPIE -s -O3 .\sources\*.cpp cs.o -lsqlite3 -o cs.exe
@del /F /Q cs.o
