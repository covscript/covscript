CXX := g++
CXX_STANDARD := c++11

LINKING_LIBRARIES := -ldl

CXX_FLAGS := -std=$(CXX_STANDARD) -fPIE -s -O3 $(LINKING_LIBRARIES)

BIN := cs
SRC_DIR = sources
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:.cpp=.o)

$(BIN): $(OBJ)
	$(CXX) -o $@ $(CXX_FLAGS) $<

%.o: %.cpp
	$(CXX) -c -o $@ $(CXX_FLAGS) $<

.PHONY: clean format
clean:
	$(RM) $(BIN)

format:
	$(RM) $(SRC_DIR)/*.gch
	astyle $(SRC_DIR)/*.*
	astyle $(SRC_DIR)/*/*.*
	$(RM) $(SRC_DIR)/*.orig
	$(RM) $(SRC_DIR)/*/*.orig
	astyle -A4 -N -t $(SRC_DIR)/*.*
	astyle -A4 -N -t $(SRC_DIR)/*/*.*
	$(RM) $(SRC_DIR)/*.orig
	$(RM) $(SRC_DIR)/*/*.orig

