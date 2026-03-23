#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL2/SDL.h>
#include "chip8.h"

#define SCALE 10
int get_key(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_1:
        return 0x1;
    case SDLK_2:
        return 0x2;
    case SDLK_3:
        return 0x3;
    case SDLK_4:
        return 0xC;
    case SDLK_q:
        return 0x4;
    case SDLK_w:
        return 0x5;
    case SDLK_e:
        return 0x6;
    case SDLK_r:
        return 0xD;
    case SDLK_a:
        return 0x7;
    case SDLK_s:
        return 0x8;
    case SDLK_d:
        return 0x9;
    case SDLK_f:
        return 0xE;
    case SDLK_z:
        return 0xA;
    case SDLK_x:
        return 0x0;
    case SDLK_c:
        return 0xB;
    case SDLK_v:
        return 0xF;
    default:
        return -1;
    }
}
int main(int argc, char *argv[])
{
    SDL_SetMainReady();

    // Initialize CHIP-8
    Chip8 cpu;
    chip8_init(&cpu);

    if (!chip8_load_rom(&cpu, "test-programs\\ibm.ch8")) //Change files
    {
        return 1;
    }

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        64 * SCALE, 32 * SCALE,
        SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Principal loop
    int running = 1;
    SDL_Event event;

    while (running)
    {
        // Manage events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;

                int key = get_key(event.key.keysym.sym);
                if (key != -1)
                    cpu.keypad[key] = 1;
            }

            if (event.type == SDL_KEYUP)
            {
                int key = get_key(event.key.keysym.sym);
                if (key != -1)
                    cpu.keypad[key] = 0;
            }
        }

        // Run emulator cycle
        chip8_cycle(&cpu);

        // Render screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black font
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white pixels

        int x, y;
        for (y = 0; y < 32; y++)
        {
            for (x = 0; x < 64; x++)
            {
                if (cpu.display[y * 64 + x])
                {
                    SDL_Rect rect = {x * SCALE, y * SCALE, SCALE, SCALE};
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(2); // ~500 cycles per second
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}