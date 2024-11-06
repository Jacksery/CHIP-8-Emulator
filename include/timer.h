#ifndef TIMER_H
#define TIMER_H

#include "chip8.h"
#include "audio.h"

void updateTimers(Chip8 &chip8, Audio &audio, double &lastTimerUpdate, const double MS_PER_TIMER);

#endif