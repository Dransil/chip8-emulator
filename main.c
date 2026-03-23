#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL2/SDL.h>
#include "chip8.h"

#define SCALE 10

int main(int argc, char *argv[]) {
    SDL_SetMainReady();

    // Initialize CHIP-8
    Chip8 cpu;
    chip8_init(&cpu);

    if (!chip8_load_rom(&cpu, "ibm.ch8")) {
        return 1;
    }

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        64 * SCALE, 32 * SCALE,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Principal loop
    int running = 1;
    SDL_Event event;

    while (running) {
        // Manage events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN)
                if (event.key.keysym.sym == SDLK_ESCAPE) running = 0;
        }

        // Run emulator cycle
        chip8_cycle(&cpu);

        // Render screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // black font
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // white pixels

        int x, y;
        for (y = 0; y < 32; y++) {
            for (x = 0; x < 64; x++) {
                if (cpu.display[y * 64 + x]) {
                    SDL_Rect rect = {x * SCALE, y * SCALE, SCALE, SCALE};
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(2);  // ~500 cycles per second
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}