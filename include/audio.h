#ifndef AUDIO_H
#define AUDIO_H

#include <SDL.h>

class Audio
{
public:
    bool initAudio();
    void audioCallback(uint8_t *stream, int len);
    void playBeep()
    {
        SDL_PauseAudioDevice(audioDevice, 0);
    }
    void stopBeep()
    {
        SDL_PauseAudioDevice(audioDevice, 1);
    }

private:
    SDL_AudioDeviceID audioDevice;
};

#endif
