@windres .\cbs2.1.rc cbs2.1.o
@g++ -std=c++11 -static -fPIE -s -O3 .\sources\main.cpp cbs2.1.o -o cbs2.1.exe
@del /F /Q cbs2.1.o
