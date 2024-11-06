#include "event_handler.h"
#include "input.h"

void handleEvents(bool &quit, Chip8 &chip8, Audio &audio, SDL_Event &e, double &lastCycleTime, double &lastTimerUpdate)
{
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDL_QUIT:
            quit = true;
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            handleKeyPress(e.key.keysym.sym, e.type == SDL_KEYDOWN, chip8.keypad);
            break;

        case SDL_DROPFILE:
        {
            // Load dropped file
            char *dropped_filedir = e.drop.file;
            if (chip8.handleDropFile(dropped_filedir))
            {
                // Reset timers when loading new ROM
                lastCycleTime = SDL_GetTicks();
                lastTimerUpdate = SDL_GetTicks();
            }
            SDL_free(dropped_filedir);
            break;
        }
        }
    }
}