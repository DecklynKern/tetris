#include <stdio.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include "../include/main.h"

const SDL_Scancode mapped_keys[NUM_HOLDABLE_KEYS] = {
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_X
};

GameData state = {0};
SDL_Renderer* renderer;

int main(int argc, char* argv[]) {

    if (argc == 1) {
        printf("Please enter a gamemode, use './tetrism list' to see all supported modes.\n");
        return 0;
    }

    if (!load_gamemode(argc, argv)) {
        printf("Invalid gamemode, use './tetrism list' to see all supported modes.\n");
        return 0;
    }

    srand(time(NULL));

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("sdl bruh: %s\n", SDL_GetError());
    }

    SDL_Window* window = SDL_CreateWindow(
        "Tetris!",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        BOARD_WIDTH * SCALE + 5 * SCALE,
        (BOARD_HEIGHT - INVISIBLE_ROWS) * SCALE + 5 * SCALE,
        0
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    init_font();
    state.gamemode.init();

    game_init();

    bool pause = false;
    bool close = false;

    while (!close) {

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_QUIT:
                close = true;
                break;

            case SDL_KEYDOWN:

                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    pause = !pause;
                    continue;
                }

                if (pause || event.key.repeat) {
                    continue;
                }

                if (event.key.keysym.scancode == mapped_keys[Input_Left]) {
                    input_left();

                } else if (event.key.keysym.scancode == mapped_keys[Input_Right]) {
                    input_right();
                
                } else if (event.key.keysym.scancode == mapped_keys[Input_InstantDrop]) {
                    input_instant_drop();

                } else if (event.key.keysym.scancode == mapped_keys[Input_Rot_CW]) {
                    input_rotate_cw();

                } else if (event.key.keysym.scancode == mapped_keys[Input_Rot_CCW]) {
                    input_rotate_ccw();
                }

                break;

            case SDL_KEYUP:

                if (pause || event.key.repeat) {
                    continue;
                }

                if (event.key.keysym.scancode == mapped_keys[Input_Left]) {
                    release_left();

                } else if (event.key.keysym.scancode == mapped_keys[Input_Right]) {
                    release_right();
                
                }
            }
        }

        for (int key = 0; key < NUM_HOLDABLE_KEYS; key++) {
            state.input_held[key] = key_state[mapped_keys[key]];
        }

        if (!pause) {
            close |= update();
        }

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        state.gamemode.draw();

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60);
        
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();

    return 0;

}