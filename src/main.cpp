#include <SDL.h>
#include "chip8.h"
#include "audio.h"
#include "input.h"
#include "sdl_utils.h"
#include "event_handler.h"
#include "timer.h"

// Emulation timings
const double CYCLES_PER_SECOND = 700.0;
const double MS_PER_CYCLE = 1000.0 / CYCLES_PER_SECOND;
const double TIMER_FREQUENCY = 60.0;
const double MS_PER_TIMER = 1000.0 / TIMER_FREQUENCY;

// Run CPU cycles and handle events, timers, and rendering
void runEmulatorLoop(Chip8 &chip8, Audio &audio, SDL_Renderer *renderer, bool &romLoaded)
{
    bool quit = false;
    SDL_Event e;

    double lastCycleTime = SDL_GetTicks();
    double lastTimerUpdate = SDL_GetTicks();

    while (!quit)
    {
        // Poll for events
        handleEvents(quit, chip8, audio, e, lastCycleTime, lastTimerUpdate);

        Uint32 currentTime = SDL_GetTicks();

        // CPU cycles
        if (romLoaded && currentTime - lastCycleTime >= MS_PER_CYCLE)
        {
            chip8.emulateCycle();
            lastCycleTime = currentTime;
        }

        updateTimers(chip8, audio, lastTimerUpdate, MS_PER_TIMER);

        if (chip8.drawFlag)
        {
            chip8.render(renderer);
            chip8.drawFlag = false;
        }
    }
}

// Initialise SDL and audio
bool initialise(Chip8 &chip8, Audio &audio, SDL_Window *&window, SDL_Renderer *&renderer)
{
    if (!initialiseSDL(window, renderer))
    {
        return false;
    }

    audio.initAudio();
    return true;
}

// Cleanup SDL and audio
void cleanup(SDL_Window *window, SDL_Renderer *renderer)
{
    cleanupSDL(window, renderer);
}

int main(int argc, char *argv[])
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    Chip8 chip8;
    Audio audio;

    if (!initialise(chip8, audio, window, renderer))
    {
        return 1;
    }

    // Message box
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Chip-8 Emulator", "Drop a Chip-8 ROM file to start emulation", window);

    // Main loop, polls until a ROM is loaded
    runEmulatorLoop(chip8, audio, renderer, chip8.romLoaded);

    cleanup(window, renderer);
    return 0;
}