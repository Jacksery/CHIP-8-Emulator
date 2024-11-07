#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <SDL.h>
#include "chip8.h"
#include "audio.h"

void handleEvents(bool &quit, Chip8 &chip8, Audio &audio, SDL_Event &e, double &lastCycleTime, double &lastTimerUpdate);

#endif