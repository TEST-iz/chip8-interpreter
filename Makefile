# --- Variables ---
CXX = g++
CXXFLAGS = -g -Wall
# Point to the 'include' folder (one level above 'SDL2')
INCLUDES = -I"C:/sdl2/include"
# Point to the 'lib' folder
LDFLAGS = -L"C:/sdl2/lib"
# The specific libraries MinGW needs
LIBS = -lmingw32 -lSDL2main -lSDL2 
# idk if need this LIBS = -lmingw32 -lSDL2main -lSDL2 -mwindows
OUT = main.exe

# --- Build Targets ---
# 'all' is the default target that runs when you just type 'make'
all:
	$(CXX) $(CXXFLAGS) src/main.cpp src/Display.cpp src/chip8.cpp -o $(OUT) $(INCLUDES) $(LDFLAGS) $(LIBS)

test:
	$(CXX) $(CXXFLAGS) -DUSING_CATCH -I/usr/include/catch2 \
	tests/test_main.cpp tests/chip8_tests.cpp src/chip8.cpp -o unit_tests
	./unit_tests

# 'clean' helps you reset your folder
clean:
	del $(OUT)
	del unit_tests