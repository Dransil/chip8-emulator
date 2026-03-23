#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    SDL_SetMainReady();
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640, 320,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    printf("SDL2 working!\n");
    SDL_Delay(2000);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}