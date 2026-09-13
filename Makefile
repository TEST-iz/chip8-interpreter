CXX = g++
CXXFLAGS = -g -Wall
INCLUDES = -I"C:/sdl2/include"
LDFLAGS = -L"C:/sdl2/lib"
LIBS = -lmingw32 -lSDL2main -lSDL2 
# idk if need this LIBS = -lmingw32 -lSDL2main -lSDL2 -mwindows
OUT = main.exe

all:
	$(CXX) $(CXXFLAGS) src/main.cpp src/Display.cpp src/Chip8.cpp -o $(OUT) $(INCLUDES) $(LDFLAGS) $(LIBS)

test:
	$(CXX) $(CXXFLAGS) -DUSING_CATCH \
	tests/tests_main.cpp \
	tests/chip8_tests.cpp \
	src/Chip8.cpp \
	-o unit_tests
	./unit_tests

clean:
	del $(OUT)
	del unit_tests