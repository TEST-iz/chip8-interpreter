#include "chip8.h"
#include "Display.h"
#include <SDL2/SDL.h>
#include <iostream>

Display display;
Chip8 chip8;

int main(int argc, char* argv[]) {
    bool quit = false;
    SDL_Event e;
    display.initialize();
    chip8.initialize();
    while (!quit) {
        quit = display.processInput(chip8.key);
    }
}