@windres .\cs.rc cs.o
@g++ -std=c++11 -static -fPIE -s -O3 .\sources\standalone.cpp cs.o -lsqlite3 -o cs.exe
@g++ -std=c++11 -static -fPIE -s -O3 .\sources\repl.cpp cs.o -lsqlite3 -o cs_repl.exe
@del /F /Q cs.o
