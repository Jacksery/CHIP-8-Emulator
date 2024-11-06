#include "chip8.h"
#include <iostream>

Chip8::Chip8()
{
    // Initial state
    pc = 0x200;
    sp = 0;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;
    drawFlag = false;
    romLoaded = false;
    memset(memory, 0, sizeof(memory));
    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    memset(gfx, 0, sizeof(gfx));
    memset(keypad, 0, sizeof(keypad));
}

bool Chip8::loadROM(const char *filename)
{
    // Open ROM file in binary mode
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cerr << "Failed to open ROM: " << filename << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > (4096 - 512))
    {
        std::cerr << "ROM is too large to fit in memory" << std::endl;
        return false;
    }

    // Load ROM into memory starting at 0x200
    file.read(reinterpret_cast<char *>(memory + 512), size);
    file.close();
    romLoaded = true;
    return true;
}

bool Chip8::handleDropFile(const char *file)
{
    // Reset emulator state
    pc = 0x200;
    sp = 0;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;
    memset(memory, 0, sizeof(memory));
    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    memset(gfx, 0, sizeof(gfx));
    memset(keypad, 0, sizeof(keypad));

    return loadROM(file);
}

void Chip8::emulateCycle()
{
    // Fetch opcode from memory at PC
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2;
    decodeOpcode(opcode);
}

void Chip8::decodeOpcode(uint16_t opcode)
{
    // Extract common components
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = opcode & 0x000F;
    uint8_t nn = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;

    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (nn)
        {
        case 0x00E0:
        {
            // Clear the screen
            memset(gfx, 0, sizeof(gfx));
            drawFlag = true;
            break;
        }
        case 0x00EE:
        {
            // Return from subroutine
            sp--;
            pc = stack[sp];
            break;
        }
        }
        break;

    case 0x1000:
        // Jump to address nnn
        pc = nnn;
        break;

    case 0x2000:
    {
        // Call subroutine at nnn
        stack[sp] = pc;
        sp++;
        pc = nnn;
        break;
    }

    case 0x3000:
        // Skip next instruction if V[x] == nn
        if (V[x] == nn)
            pc += 2;
        break;

    case 0x4000:
        // Skip next instruction if V[x] != nn
        if (V[x] != nn)
            pc += 2;
        break;

    case 0x5000:
        // Skip next instruction if V[x] == V[y]
        if (V[x] == V[y])
            pc += 2;
        break;

    case 0x6000:
        // Set V[x] to nn
        V[x] = nn;
        break;

    case 0x7000:
        // Add nn to V[x]
        V[x] += nn;
        break;

    // Arithmetic operations
    case 0x8000:
        switch (n)
        {
        case 0x0000:
            // Set V[x] to V[y]
            V[x] = V[y];
            break;

        case 0x0001:
            // Set V[x] to V[x] OR V[y]
            V[x] |= V[y];
            break;

        case 0x0002:
            // Set V[x] to V[x] AND V[y]
            V[x] &= V[y];
            break;

        case 0x0003:
            // Set V[x] to V[x] XOR V[y]
            V[x] ^= V[y];
            break;

        case 0x0004:
        {
            // Add V[y] to V[x], set V[F] to 1 if there is a carry
            uint8_t vx = V[x];
            uint8_t vy = V[y];
            V[x] += vy;
            V[0xF] = (vx > 0xFF - vy) ? 1 : 0;
            break;
        }

        case 0x0005:
        {
            // Subtract V[y] from V[x], set V[F] to 0 if there is a borrow
            uint8_t vx = V[x];
            uint8_t vy = V[y];
            V[x] -= vy;
            V[0xF] = (vx >= vy) ? 1 : 0;
            break;
        }

        case 0x0006:
        {
            // Shift V[x] right by 1, set V[F] to the least significant bit
            uint8_t vx = V[x];
            V[x] >>= 1;
            V[0xF] = vx & 0x1;
            break;
        }

        case 0x0007:
        {
            // Set V[x] to V[y] - V[x], set V[F] to 0 if there is a borrow
            uint8_t vx = V[x];
            uint8_t vy = V[y];
            V[x] = vy - vx;
            V[0xF] = (vy >= vx) ? 1 : 0;
            break;
        }

        case 0x000E:
        {
            // Shift V[x] left by 1, set V[F] to the most significant bit
            uint8_t vx = V[x];
            V[x] <<= 1;
            V[0xF] = (vx >> 7) & 0x1;
            break;
        }
        }
        break;

    case 0x9000:
        // Skip next instruction if V[x] != V[y]
        if (V[x] != V[y])
            pc += 2;
        break;

    case 0xA000:
        // Set I to nnn
        I = nnn;
        break;

    case 0xB000:
        // Jump to address nnn + V[0]
        pc = nnn + V[0];
        break;

    case 0xC000:
        // Set V[x] to a random number AND nn
        V[x] = (rand() % 256) & nn;
        break;

    case 0xD000:
    {
        // Draw sprite at (V[x], V[y]) with width 8 and height n
        // Sprite data is read from memory starting at address I
        uint8_t xCoord = V[x];
        uint8_t yCoord = V[y];
        uint8_t height = n;
        V[0xF] = 0;

        for (int row = 0; row < height; row++)
        {
            uint8_t pixel = memory[I + row];
            for (int col = 0; col < 8; col++)
            {
                if (pixel & (0x80 >> col))
                {
                    uint16_t index = xCoord + col + ((yCoord + row) * 64);
                    if (gfx[index] == 1)
                        V[0xF] = 1;
                    gfx[index] ^= 1;
                }
            }
        }
        drawFlag = true;
        break;
    }

    case 0xE000:
        // Handle key press events
        switch (nn)
        {
        case 0x009E:
            // Skip next instruction if key V[x] is pressed
            if (keypad[V[x]])
                pc += 2;
            break;

        case 0x00A1:
            // Skip next instruction if key V[x] is not pressed
            if (!keypad[V[x]])
                pc += 2;
            break;
        }
        break;

    case 0xF000:
        switch (nn)
        {
        case 0x0007:
            // Set V[x] to delay timer
            V[x] = delay_timer;
            break;

        case 0x000A:
        {
            // Wait for key press, store key in V[x]
            bool keyPressed = false;
            for (int i = 0; i < 16; i++)
            {
                if (keypad[i])
                {
                    V[x] = i;
                    keyPressed = true;
                    break;
                }
            }
            // If no key was pressed, repeat instruction
            if (!keyPressed)
                pc -= 2;
            break;
        }

        case 0x0015:
            // Set delay timer to V[x]
            delay_timer = V[x];
            break;

        case 0x0018:
            // Set sound timer to V[x]
            sound_timer = V[x];
            break;

        case 0x001E:
            // Add V[x] to I
            I += V[x];
            break;

        case 0x0029:
            // Set I to the location of the sprite for the character in V[x]
            // Used for font sprites in memory locations 0x000 to 0x01F
            I = V[x] * 5;
            break;

        case 0x0033:
        {
            // Store BCD representation of V[x] in memory locations I, I+1, I+2
            uint8_t value = V[x];
            memory[I] = value / 100;
            memory[I + 1] = (value / 10) % 10;
            memory[I + 2] = value % 10;
            break;
        }

        case 0x0055:
            // Store V[0] to V[x] in memory starting at address I
            for (int i = 0; i <= x; i++)
            {
                memory[I + i] = V[i];
            }
            break;

        case 0x0065:
            // Read V[0] to V[x] from memory starting at address I
            for (int i = 0; i <= x; i++)
            {
                V[i] = memory[I + i];
            }
            break;
        }
        break;
    }
}

void Chip8::render(SDL_Renderer *renderer)
{
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw pixels
    SDL_SetRenderDrawColor(renderer, 40, 255, 70, 255);
    for (int y = 0; y < CHIP8_HEIGHT; ++y)
    {
        for (int x = 0; x < CHIP8_WIDTH; ++x)
        {
            if (gfx[(y * CHIP8_WIDTH) + x] == 1)
            {
                // Scale up chip8 pixels to window size
                SDL_Rect pixel = {x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE};
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}