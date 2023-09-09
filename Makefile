CXX = g++
CXXFLAGS = -std=c++14 -fPIC -I./include -I./include/imgui -I/usr/include/SDL2 -ggdb3
LDFLAGS = -shared -ldl -lSDL2 -lGL

SOURCES=$(shell find include/ -type f -iname '*.cpp')
OBJECTS=$(SOURCES:.cpp=.o)

TYPE = src/runtime-x86
OUT := libsdl-imgui.so


x86: 32bit_flags build
	

32bit_flags:
	$(eval CXXFLAGS += -m32)
	$(eval LDFLAGS += -m32)


clean:
	rm -vf $(OBJECTS) $(OUT)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

build: $(OBJECTS) $(TYPE)/init.o
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(TYPE)/init.o -o $(OUT) $(LDFLAGS)
