#include "audio.h"
#include <iostream>

bool Audio::initAudio()
{
    // Set up audio device
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = 44100;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = [](void *userdata, uint8_t *stream, int len)
    {
        static_cast<Audio *>(userdata)->audioCallback(stream, len);
    };

    audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, NULL, 0);
    if (audioDevice == 0)
    {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void Audio::audioCallback(uint8_t *stream, int len)
{
    static int phase = 0;
    int16_t *buffer = (int16_t *)stream;
    int length = len / 2;

    int beepFrequency = 330;

    // Generate square wave
    for (int i = 0; i < length; i++)
    {
        buffer[i] = (phase < 44100 / (2 * beepFrequency)) ? 3000 : -3000;
        phase = (phase + 1) % (44100 / beepFrequency);
    }
}
