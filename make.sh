mkdir -p build
cd build
mkdir -p bin
g++ -std=c++11 -I ../include -fPIE -s -O3 -DCS_IMPORT_PATH="/etc/covscript/imports" ../sources/standalone.cpp -ldl -o ./bin/cs &
g++ -std=c++11 -I ../include -fPIE -s -O3 -DCS_IMPORT_PATH="/etc/covscript/imports" ../sources/repl.cpp -ldl -o ./bin/cs_repl &
wait