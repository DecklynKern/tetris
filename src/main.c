#include <stdio.h>
#include <time.h>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define SDL_FLAGS SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC | SDL_INIT_AUDIO)

#else
    #define SDL_FLAGS SDL_INIT_EVERYTHING
#endif

#include "../include/main.h"

const SDL_Scancode mapped_keys[NUM_HOLDABLE_KEYS] = {
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_X,
    SDL_SCANCODE_C
};

GameData state = {0};
SDL_Renderer* renderer;

static bool pause = true;
static bool close = false;
static const Uint8* key_state;
static clock_t start_time;

void tick(void) {

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
            
            } else if (event.key.keysym.scancode == mapped_keys[Input_Hold]) {
                input_hold();
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

    state.timer_ms = ((clock() - start_time) * 1000) / CLOCKS_PER_SEC;

    if (!pause) {
        close |= update();
    }

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    state.gamemode.draw();

    if (pause) {
        draw_large_text(SCALE * 3, TOP_SPACE_HEIGHT + SCALE * 8, "PAUSED");
        draw_large_text(SCALE * 2.4, TOP_SPACE_HEIGHT + SCALE * 9, "[Press ESC]");
    }

    SDL_RenderPresent(renderer);

}

int main(int argc, char* argv[]) {

    #ifdef __EMSCRIPTEN__
    argc = 2;
    argv[1] = "tgm1";
    #endif

    if (argc == 1) {
        printf("Please enter a gamemode, use './tetrism list' to see all supported modes.\n");
        return 0;
    }

    if (!load_gamemode(argc, argv)) {
        return 0;
    }

    state.gamemode.init();
    game_init();

    srand(time(NULL));

    if (SDL_Init(SDL_FLAGS)) {
        printf("sdl bruh: %s\n", SDL_GetError());
    }

    SDL_Window* window = SDL_CreateWindow(
        "Tetris!",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        BOARD_WIDTH * SCALE + TOP_SPACE_HEIGHT,
        (BOARD_HEIGHT - INVISIBLE_ROWS) * SCALE + RIGHT_SPACE_WIDTH,
        0
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    key_state = SDL_GetKeyboardState(NULL);

    init_fonts();

    start_time = clock();

    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(tick, 60, false);

    #else

        while (!close) {
            tick();
            SDL_Delay(1000 / 60);
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        
        SDL_Quit();

    #endif

    return 0;

}