#ifndef CHIP8_H
#define CHIP8_H

#include <SDL.h>
#include <cstdint>
#include <fstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;
const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;
const int PIXEL_SIZE = 10;

// Chip-8 fontset
const unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class Chip8
{
public:
    Chip8();
    bool loadROM(const char *filename);
    void emulateCycle();
    void render(SDL_Renderer *renderer);
    uint8_t keypad[16];
    bool drawFlag;
    bool romLoaded;
    bool handleDropFile(const char *filename);

    uint8_t getDelayTimer() const { return delay_timer; }
    uint8_t getSoundTimer() const { return sound_timer; }
    void decrementDelayTimer()
    {
        if (delay_timer > 0)
            delay_timer--;
    }
    void decrementSoundTimer()
    {
        if (sound_timer > 0)
            sound_timer--;
    }

private:
    void decodeOpcode(uint16_t opcode);

    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t stack[16];
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t I;
    uint16_t pc;
    uint8_t gfx[CHIP8_WIDTH * CHIP8_HEIGHT];
};

#endif
