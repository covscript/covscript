g++ -std=c++11 -fPIE -s -O3 ./sources/standalone.cpp -ldl -lsqlite3 -o cs
g++ -std=c++11 -fPIE -s -O3 ./sources/repl.cpp -ldl -lsqlite3 -o cs_repl
