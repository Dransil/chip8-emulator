#include <stdio.h>
#include "chip8.h"

int main() {
    Chip8 cpu;

    chip8_init(&cpu);

    // Load a test program directly into memory
    // This program: clean screen, jump to 0x200 (infinite loop)
    cpu.memory[0x200] = 0x00;
    cpu.memory[0x201] = 0xE0;  // 0x00E0 - clear screen
    cpu.memory[0x202] = 0x12;
    cpu.memory[0x203] = 0x00;  // 0x1200 - jump to 0x200

    // Run 4 cycles
    int i;
    for (i = 0; i < 4; i++) {
        chip8_cycle(&cpu);
    }

    return 0;
}