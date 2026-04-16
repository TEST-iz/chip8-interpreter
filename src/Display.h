#ifndef Display_H
#define Display_H
#include <SDL2/SDL.h>
#include <iostream>

class Display {
    public:
        bool initialize();
        bool processInput(unsigned char* keys);
    private:
        SDL_Window* window;
};

#endif