#include <stdio.h>
#include <string.h>
#include "chip8.h"
#include <stdlib.h>
#include <time.h>
// Built-in font (0-F)
uint8_t fontset[80] = {
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

void chip8_init(Chip8 *cpu)
{
    srand(time(NULL));
    // Clean all
    memset(cpu, 0, sizeof(Chip8));

    // The program always start at 0x200
    cpu->pc = 0x200;

    // Load the memory on 0x000
    memcpy(cpu->memory, fontset, sizeof(fontset));

    printf("CHIP-8 initialized\n");
}

int chip8_load_rom(Chip8 *cpu, const char *filename)
{
    FILE *rom = fopen(filename, "rb");

    if (rom == NULL)
    {
        printf("Error: could not open ROM: %s\n", filename);
        return 0;
    }

    // Load ROM memory from 0x200
    int bytes = fread(&cpu->memory[0x200], 1, 4096 - 0x200, rom);
    fclose(rom);

    printf("ROM loaded: %d bytes\n", bytes);
    return 1;
}

void chip8_cycle(Chip8 *cpu)
{
    // Fetch
    cpu->opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];
    cpu->pc += 2;

    // Extract opcode components
    uint8_t X = (cpu->opcode & 0x0F00) >> 8;
    uint8_t Y = (cpu->opcode & 0x00F0) >> 4;
    uint8_t NN = cpu->opcode & 0x00FF;
    uint16_t NNN = cpu->opcode & 0x0FFF;
    uint8_t N = cpu->opcode & 0x000F;

    // Decode & Execute
    switch (cpu->opcode & 0xF000)
    {

    case 0x0000:
        switch (cpu->opcode)
        {
        case 0x00E0:
            // Clean screen
            memset(cpu->display, 0, sizeof(cpu->display));
            break;
        case 0x00EE:
            // Subroutine return
            cpu->sp--;
            cpu->pc = cpu->stack[cpu->sp];
            break;
        }
        break;

    case 0x1000:
        // Jump to NNN
        cpu->pc = NNN;
        break;

    case 0x2000:
        // Call subroutine on NNN
        cpu->stack[cpu->sp] = cpu->pc;
        cpu->sp++;
        cpu->pc = NNN;
        break;

    case 0x3000:
        // Jump if VX == NN
        if (cpu->V[X] == NN)
            cpu->pc += 2;
        break;

    case 0x4000:
        // Jump if VX != NN
        if (cpu->V[X] != NN)
            cpu->pc += 2;
        break;

    case 0x5000:
        // Jump if VX == VY
        if (cpu->V[X] == cpu->V[Y])
            cpu->pc += 2;
        break;

    case 0x6000:
        // VX = NN
        cpu->V[X] = NN;
        break;

    case 0x7000:
        // VX += NN
        cpu->V[X] += NN;
        break;

    case 0x8000:
        switch (N)
        {
        case 0x0:
            cpu->V[X] = cpu->V[Y];
            break;
        case 0x1:
            cpu->V[X] |= cpu->V[Y];
            break;
        case 0x2:
            cpu->V[X] &= cpu->V[Y];
            break;
        case 0x3:
            cpu->V[X] ^= cpu->V[Y];
            break;
        case 0x4:
            // VX += VY with carry
            cpu->V[0xF] = (cpu->V[X] + cpu->V[Y] > 255) ? 1 : 0;
            cpu->V[X] += cpu->V[Y];
            break;
        case 0x5:
            // VX -= VY with borrow
            cpu->V[0xF] = (cpu->V[X] > cpu->V[Y]) ? 1 : 0;
            cpu->V[X] -= cpu->V[Y];
            break;
        case 0x6:
            // VX >>= 1
            cpu->V[0xF] = cpu->V[X] & 0x1;
            cpu->V[X] >>= 1;
            break;
        case 0x7:
            // VX = VY - VX
            cpu->V[0xF] = (cpu->V[Y] > cpu->V[X]) ? 1 : 0;
            cpu->V[X] = cpu->V[Y] - cpu->V[X];
            break;
        case 0xE:
            // VX <<= 1
            cpu->V[0xF] = (cpu->V[X] >> 7) & 0x1;
            cpu->V[X] <<= 1;
            break;
        }
        break;

    case 0x9000:
        // Jump if VX != VY
        if (cpu->V[X] != cpu->V[Y])
            cpu->pc += 2;
        break;

    case 0xA000:
        // I = NNN
        cpu->I = NNN;
        break;

    case 0xB000:
        // Jump to NNN + V0
        cpu->pc = NNN + cpu->V[0];
        break;

    case 0xF000:
        switch (NN)
        {
        case 0x07:
            // VX = delay_timer
            cpu->V[X] = cpu->delay_timer;
            break;
        case 0x15:
            // delay_timer = VX
            cpu->delay_timer = cpu->V[X];
            break;
        case 0x18:
            // sound_timer = VX
            cpu->sound_timer = cpu->V[X];
            break;
        case 0x1E:
            // I += VX
            cpu->I += cpu->V[X];
            break;
        case 0x29:
            // I = VX digit sprite direction
            cpu->I = cpu->V[X] * 5;
            break;
        case 0x33:
            // BCD de VX
            cpu->memory[cpu->I] = cpu->V[X] / 100;
            cpu->memory[cpu->I + 1] = (cpu->V[X] / 10) % 10;
            cpu->memory[cpu->I + 2] = cpu->V[X] % 10;
            break;
        case 0x55:
            // Save V0-VX on memory
            {
                int i;
                for (i = 0; i <= X; i++)
                    cpu->memory[cpu->I + i] = cpu->V[i];
            }
            break;
        case 0x65:
            // Read V0-VX from memory
            {
                int i;
                for (i = 0; i <= X; i++)
                    cpu->V[i] = cpu->memory[cpu->I + i];
            }
            break;
        case 0x0A:
        {
            // Wait for key press, store in VX
            int pressed = -1;
            int i;
            for (i = 0; i < 16; i++)
            {
                if (cpu->keypad[i])
                {
                    pressed = i;
                    break;
                }
            }
            // If no key pressed, repeat this instruction
            if (pressed == -1)
                cpu->pc -= 2;
            else
                cpu->V[X] = pressed;
            break;
        }
        }
        break;
    case 0xD000:
    {
        // Draw sprite at (VX, VY) with N bytes of height
        uint8_t xpos = cpu->V[X] % 64;
        uint8_t ypos = cpu->V[Y] % 32;
        uint8_t height = N;
        cpu->V[0xF] = 0;

        int row, col;
        for (row = 0; row < height; row++)
        {
            uint8_t sprite = cpu->memory[cpu->I + row];

            for (col = 0; col < 8; col++)
            {
                // If current sprite bit is active
                if (sprite & (0x80 >> col))
                {
                    int px = (xpos + col) % 64;
                    int py = (ypos + row) % 32;
                    int idx = py * 64 + px;

                    // If pixel was already active, collision detected → VF = 1
                    if (cpu->display[idx])
                        cpu->V[0xF] = 1;

                    // XOR the pixel
                    cpu->display[idx] ^= 1;
                }
            }
        }
        break;
    }
    case 0xC000:
        // VX = random number & NN
        cpu->V[X] = (rand() % 256) & NN;
        break;
    case 0xE000:
        switch (NN)
        {
        case 0x9E:
            // Skip if key VX is pressed
            if (cpu->keypad[cpu->V[X]])
                cpu->pc += 2;
            break;
        case 0xA1:
            // Skip if key VX is not pressed
            if (!cpu->keypad[cpu->V[X]])
                cpu->pc += 2;
            break;
        }
        break;

    default:
        printf("Unknown opcode: 0x%04X\n", cpu->opcode);
        break;
    }

    // Decrement timers
    if (cpu->delay_timer > 0)
        cpu->delay_timer--;
    if (cpu->sound_timer > 0)
        cpu->sound_timer--;
}

// void chip8_cycle(Chip8 *cpu) {
//     // Fetch — Read opcode
//     cpu->opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];

//     printf("PC: 0x%04X | Opcode: 0x%04X\n", cpu->pc, cpu->opcode);

//     // Increment the program counter
//     cpu->pc += 2;

//     // Decode & Execute
//     switch (cpu->opcode & 0xF000) {
//         case 0x0000:
//             if (cpu->opcode == 0x00E0) {
//                 // Limpiar pantalla
//                 memset(cpu->display, 0, sizeof(cpu->display));
//                 printf("  -> Clear screen\n");
//             }
//             break;

//         case 0x1000:
//             // Jump to direction NNN
//             cpu->pc = cpu->opcode & 0x0FFF;
//             printf("  -> Jump to 0x%03X\n", cpu->pc);
//             break;

//         case 0x6000:
//             // Save NN on register VX
//             cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->opcode & 0x00FF;
//             printf("  -> Set V%X = 0x%02X\n",
//                 (cpu->opcode & 0x0F00) >> 8,
//                 (cpu->opcode & 0x00FF));
//             break;

//         default:
//             printf("  -> Unknown opcode\n");
//             break;
//     }
// }