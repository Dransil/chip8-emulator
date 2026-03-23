# CHIP-8 Emulator

A fully functional CHIP-8 emulator written in C from scratch.

## Features
- Full CPU implementation (35 opcodes)
- 4KB RAM
- 64x32 pixel display
- Hexadecimal keypad input
- Delay and sound timers
- SDL2 rendering

## How to compile
gcc main.c chip8.c -o chip8 -IC:/msys64/mingw64/include \
    -LC:/msys64/mingw64/lib -lSDL2

## Controls
| CHIP-8 | Keyboard |
|--------|----------|
| 1 2 3 C | 1 2 3 4 |
| 4 5 6 D | Q W E R |
| 7 8 9 E | A S D F |
| A 0 B F | Z X C V |
