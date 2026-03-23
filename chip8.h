#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

// CHIP-8 components
typedef struct {
    uint8_t  memory[4096];   // 4KB de RAM
    uint8_t  V[16];          // 16 8-bit registers (V0-VF)
    uint16_t opcode;         // actual instruction
    uint16_t pc;             // program counter
    uint16_t stack[16];      // stack
    uint8_t  sp;             // stack pointer
    uint8_t  display[64*32]; // screen 64x32
    uint8_t  delay_timer;    // delay timer
    uint8_t  sound_timer;    // sound timer
    uint8_t  keypad[16];     // 16 keys
    uint16_t I;              // index record
} Chip8;

// Functions
void chip8_init(Chip8 *cpu);
int  chip8_load_rom(Chip8 *cpu, const char *filename);
void chip8_cycle(Chip8 *cpu);

#endif