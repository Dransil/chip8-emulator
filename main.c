#include <stdio.h>
#include "chip8.h"

int main() {
    Chip8 cpu;
    chip8_init(&cpu);

    // Opcodes test
    // 0x6A05 → V[A] = 5
    cpu.memory[0x200] = 0x6A;
    cpu.memory[0x201] = 0x05;

    // 0x6B03 → V[B] = 3
    cpu.memory[0x202] = 0x6B;
    cpu.memory[0x203] = 0x03;

    // 0x8AB4 → V[A] += V[B] with carry
    cpu.memory[0x204] = 0x8A;
    cpu.memory[0x205] = 0xB4;

    // 0xA300 → I = 0x300
    cpu.memory[0x206] = 0xA3;
    cpu.memory[0x207] = 0x00;

    // Run 4 cycles
    int i;
    for (i = 0; i < 4; i++) {
        chip8_cycle(&cpu);
    }

    // Result
    printf("\n--- Result ---\n");
    printf("V[A] = %d\n", cpu.V[0xA]);
    printf("V[B] = %d\n", cpu.V[0xB]);
    printf("V[F] = %d (carry)\n", cpu.V[0xF]);
    printf("I    = 0x%03X\n", cpu.I);

    return 0;
}