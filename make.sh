mkdir build
cd build
mkdir bin
mkdir imports
g++ -std=c++11 -fPIE -s -O3 ../sources/standalone.cpp -ldl -o ./bin/cs
g++ -std=c++11 -fPIE -s -O3 ../sources/repl.cpp -ldl -o ./bin/cs_repl
g++ -std=c++11 -shared -fPIC -s -O3 ../sources/extensions/darwin.cpp -o ./imports/darwin.cse
g++ -std=c++11 -shared -fPIC -s -O3 ../sources/extensions/sqlite.cpp -lsqlite3 -o ./imports/sqlite.cse
