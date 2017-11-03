@mkdir build
@cd build
@mkdir bin
@mkdir imports
@windres ..\cs.rc cs.o
@g++ -std=c++11 -static -fPIE -s -O3 ..\sources\standalone.cpp cs.o -o .\bin\cs.exe
@g++ -std=c++11 -static -fPIE -s -O3 ..\sources\repl.cpp cs.o -o .\bin\cs_repl.exe
@g++ -std=c++11 -shared -fPIC -s -O3 ..\sources\extensions\darwin.cpp -o .\imports\darwin.cse
@g++ -std=c++11 -shared -fPIC -s -O3 ..\sources\extensions\sqlite.cpp -lsqlite3 -o .\imports\sqlite.cse
@del /F /Q cs.o
