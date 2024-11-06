#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL2/SDL.h>

bool initialiseSDL(SDL_Window *&window, SDL_Renderer *&renderer);
void cleanupSDL(SDL_Window *window, SDL_Renderer *renderer);

#endif