#include "timer.h"
#include <SDL2/SDL.h>

void updateTimers(Chip8 &chip8, Audio &audio, double &lastTimerUpdate, const double MS_PER_TIMER)
{
    // Get current time
    Uint32 currentTime = SDL_GetTicks();

    // Timer updates at 60Hz
    if (currentTime - lastTimerUpdate >= MS_PER_TIMER)
    {
        if (chip8.getDelayTimer() > 0)
        {
            chip8.decrementDelayTimer();
        }

        if (chip8.getSoundTimer() > 0)
        {
            chip8.decrementSoundTimer();
            audio.playBeep();
        }
        else
        {
            audio.stopBeep();
        }

        lastTimerUpdate += MS_PER_TIMER;
    }
}