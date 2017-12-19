mkdir build
cd build
mkdir bin
g++ -std=c++11 -I ../include -fPIE -s -O3 ../sources/standalone.cpp -ldl -o ./bin/cs
g++ -std=c++11 -I ../include -fPIE -s -O3 ../sources/repl.cpp -ldl -o ./bin/cs_repl