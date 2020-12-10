EXE = db
SRC_DIR = src
SRC = $(shell find src/ -name "*.cc")
OBJ = $(SRC:%.cc=%.o)
HEADERS = $(shell find src/ -name "*.h")
CPPFLAGS += -I $(SRC_DIR) 
CFLAGS += -g -std=c++17 -Wall -fmax-errors=5 -lstdc++fs
LDLIBS = -lstdc++fs -lreadline

all: main

main: $(OBJ)
	mkdir -p bin
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o bin/$(EXE)

%.o: %.cc $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: all clean release

clean:
	find . -name "*.o" -type f -delete
	rm -f *~ core $(SRC_DIR)/*~