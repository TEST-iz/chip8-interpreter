#ifndef Display_H
#define Display_H
#include <SDL2/SDL.h>
#include <iostream>

class Display {
    public:
        bool initialize();
        bool processInput(unsigned char* keys);
        bool draw(unsigned int* gfx);
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
};

#endif