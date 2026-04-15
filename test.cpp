#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Chip-8 Test", SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);
        
    if (window == nullptr) {
        return 1;
    }

    SDL_Delay(2000); // Show window for 2 seconds
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
