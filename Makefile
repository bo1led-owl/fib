SOURCES = src/main.cc src/number.cc
FLAGS = -std=c++23 -Wall -Wextra -Wpedantic
OPTFLAGS = -O3 -fno-rtti -flto -march=native -DNDEBUG

linear: ./build/linear
matexp: ./build/matexp

./build/linear: ./build $(SOURCES)
	$(CXX) $(FLAGS) $(OPTFLAGS) $(SOURCES) -o $@ -DLINEAR

./build/matexp: ./build $(SOURCES)
	$(CXX) $(FLAGS) $(OPTFLAGS) $(SOURCES) -o $@ -DMATEXP

./build:
	mkdir build

clean:
	rm -r ./build
	
.PHONY: linear matexp clean
