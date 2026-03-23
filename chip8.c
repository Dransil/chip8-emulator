#include <stdio.h>
#include <string.h>
#include "chip8.h"

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

void chip8_init(Chip8 *cpu) {
    // Clean all
    memset(cpu, 0, sizeof(Chip8));

    // The program always start at 0x200
    cpu->pc = 0x200;

    // Load the memory on 0x000
    memcpy(cpu->memory, fontset, sizeof(fontset));

    printf("CHIP-8 initialized\n");
}

int chip8_load_rom(Chip8 *cpu, const char *filename) {
    FILE *rom = fopen(filename, "rb");

    if (rom == NULL) {
        printf("Error: could not open ROM: %s\n", filename);
        return 0;
    }

    // Load ROM memory from 0x200
    int bytes = fread(&cpu->memory[0x200], 1, 4096 - 0x200, rom);
    fclose(rom);

    printf("ROM loaded: %d bytes\n", bytes);
    return 1;
}

void chip8_cycle(Chip8 *cpu) {
    // Fetch — Read opcode
    cpu->opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];

    printf("PC: 0x%04X | Opcode: 0x%04X\n", cpu->pc, cpu->opcode);

    // Increment the program counter
    cpu->pc += 2;

    // Decode & Execute
    switch (cpu->opcode & 0xF000) {
        case 0x0000:
            if (cpu->opcode == 0x00E0) {
                // Limpiar pantalla
                memset(cpu->display, 0, sizeof(cpu->display));
                printf("  -> Clear screen\n");
            }
            break;

        case 0x1000:
            // Jump to direction NNN
            cpu->pc = cpu->opcode & 0x0FFF;
            printf("  -> Jump to 0x%03X\n", cpu->pc);
            break;

        case 0x6000:
            // Save NN on register VX
            cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->opcode & 0x00FF;
            printf("  -> Set V%X = 0x%02X\n",
                (cpu->opcode & 0x0F00) >> 8,
                (cpu->opcode & 0x00FF));
            break;

        default:
            printf("  -> Unknown opcode\n");
            break;
    }
}