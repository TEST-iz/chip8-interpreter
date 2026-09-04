#include "Display.h"
#include <iostream>
#include <system_error>
#include <SDL2/SDL.h>

using namespace std;

bool Display::initialize() {
    if (SDL_Init (SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }
    else {
        window = SDL_CreateWindow("Chip-8 Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 640, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            cout << "Window could not be created! SDL_Error: %s\n" << SDL_GetError();
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) {
            cout << "Renderer could not be created! SDL_Error: %s\n" << SDL_GetError();
            return false;
        }
        
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
        if (texture == NULL) {
            cout << "Texture could not be created! SDL_Error: %s\n" << SDL_GetError();
            return false;
        }
        return true;
    }
}

bool Display::draw(unsigned int* gfx) {
    SDL_UpdateTexture(texture, nullptr, gfx, 64 * sizeof(unsigned int));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    return true;
    }  

bool Display::processInput(unsigned char* keys) {
    SDL_Event e;
    bool quit = false;
    while (SDL_PollEvent( &e ) != 0) {
        switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                        
                    case SDLK_x:
                        keys[0] = 1;
                        break;

                    case SDLK_1:
                        keys[1] = 1;
                        break;

                    case SDLK_2:
                        keys[2] = 1;
                        break;

                    case SDLK_3:
                        keys[3] = 1;
                        break;

                    case SDLK_q:
                        keys[4] = 1;
                        break;

                    case SDLK_w:
                        keys[5] = 1;
                        break;

                    case SDLK_e:
                        keys[6] = 1;
                        break;

                    case SDLK_a:
                        keys[7] = 1;
                        break;

                    case SDLK_s:
                        keys[8] = 1;
                        break;

                    case SDLK_d:
                        keys[9] = 1;
                        break;

                    case SDLK_z:
                        keys[10] = 1;
                        break;

                    case SDLK_c:
                        keys[11] = 1;
                        break;

                    case SDLK_4:
                        keys[12] = 1;
                        break;

                    case SDLK_r:
                        keys[13] = 1;
                        break;

                    case SDLK_f:
                        keys[14] = 1;
                        break;

                    case SDLK_v:
                        keys[15] = 1;
                        break;
                }
                break;
            
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                    case SDLK_x:
                        keys[0] = 0;
                        break;

                    case SDLK_1:
                        keys[1] = 0;
                        break;

                    case SDLK_2:
                        keys[2] = 0;
                        break;

                    case SDLK_3:
                        keys[3] = 0;
                        break;

                    case SDLK_q:
                        keys[4] = 0;
                        break;

                    case SDLK_w:
                        keys[5] = 0;
                        break;

                    case SDLK_e:
                        keys[6] = 0;
                        break;

                    case SDLK_a:
                        keys[7] = 0;
                        break;

                    case SDLK_s:
                        keys[8] = 0;
                        break;

                    case SDLK_d:
                        keys[9] = 0;
                        break;

                    case SDLK_z:
                        keys[10] =0;
                        break;

                    case SDLK_c:
                        keys[11] = 0;
                        break;

                    case SDLK_4:
                        keys[12] = 0;
                        break;

                    case SDLK_r:
                        keys[13] = 0;
                        break;

                    case SDLK_f:
                        keys[14] = 0;
                        break;

                    case SDLK_v:
                        keys[15] = 0;
                        break;
                }
                break;
        }
    }
    return quit;
}