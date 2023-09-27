#include <SDL2/SDL_scancode.h>
#include <stdio.h>
#include <sys/time.h>
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

typedef enum {
    MainMenu,
    InGame,
    Paused,
    Closing
} MenuState;

GameData state = {0};
SDL_Renderer* renderer;

static MenuState menu_state = MainMenu;
static int selected_gamemode = 0;
static const Uint8* key_state;

static double timer_seconds_before_last_pause;
static struct timeval last_unpause_time;

static double seconds_since_last_unpause(void) {

    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    return current_time.tv_sec - last_unpause_time.tv_sec + 
        (double) (current_time.tv_usec - last_unpause_time.tv_usec) / 1000000;

}

void tick(void) {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {

        case SDL_QUIT:
            menu_state = Closing;
            break;

        case SDL_KEYDOWN:

            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                
                if (menu_state == InGame) {
                    timer_seconds_before_last_pause += seconds_since_last_unpause();
                    menu_state = Paused;
                }
                else if (menu_state == Paused) {
                    gettimeofday(&last_unpause_time, NULL);
                    menu_state = InGame;
                }

                continue;

            }

            if (menu_state == MainMenu) {

                if (
                    event.key.keysym.scancode == SDL_SCANCODE_DOWN &&
                    selected_gamemode < NUM_GAMEMODES - 1
                ) {
                    selected_gamemode++;
                }
                else if (
                    event.key.keysym.scancode == SDL_SCANCODE_UP &&
                    selected_gamemode > 0
                ) {
                    selected_gamemode--;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {

                    load_gamemode(selected_gamemode);

                    state.gamemode.init();
                    game_init();

                    menu_state = Paused;

                }
            }

            if (menu_state != InGame || event.key.repeat) {
                continue;
            }

            if (event.key.keysym.scancode == mapped_keys[Input_Left]) {
                input_left();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_Right]) {
                input_right();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_Down]) {
                input_down();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_InstantDrop]) {
                input_instant_drop();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_Rot_CW]) {
                input_rotate_cw();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_Rot_CCW]) {
                input_rotate_ccw();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_Hold]) {
                input_hold();
            }

            break;

        case SDL_KEYUP:

            if (menu_state != InGame || event.key.repeat) {
                continue;
            }

            if (event.key.keysym.scancode == mapped_keys[Input_Left]) {
                release_left();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_Right]) {
                release_right();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_Down]) {
                release_down();
            }
        }
    }

    if (menu_state != MainMenu) {

        for (int key = 0; key < NUM_HOLDABLE_KEYS; key++) {
            state.input_held[key] = key_state[mapped_keys[key]];
        }
    }

    if (menu_state == InGame) {

        state.timer_ms = (timer_seconds_before_last_pause + seconds_since_last_unpause()) * 1000;
        
        if (update()) {
            menu_state = Closing;
        }
    }

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    if (menu_state == MainMenu) {

        draw_large_text(SCALE * 5, 0, "SELECT GAME");

        for (int i = 0; i < NUM_GAMEMODES; i++) {

            int y = LARGE_FONT_SIZE * (i + 1);

            if (selected_gamemode == i) {
                
                SDL_Rect rect = {
                    0,
                    y,
                    BOARD_WIDTH * SCALE + TOP_SPACE_HEIGHT,
                    LARGE_FONT_SIZE
                };
                
                SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &rect);
            
            }

            draw_large_text(SCALE * 5, y, gamemode_names[i]);
        
        }
    }
    else {

        state.gamemode.draw();

        if (menu_state == Paused) {
            draw_large_text(SCALE * 3, TOP_SPACE_HEIGHT + SCALE * 8, "PAUSED");
            draw_large_text(SCALE * 2.4, TOP_SPACE_HEIGHT + SCALE * 9, "[Press ESC]");
        }
    }


    SDL_RenderPresent(renderer);

}

int main(void) {

    srand(time(NULL));

    if (SDL_Init(SDL_FLAGS)) {
        printf("sdl bruh: %s\n", SDL_GetError());
    }

    SDL_Window* window = SDL_CreateWindow(
        "Tetris!",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        BOARD_WIDTH * SCALE + RIGHT_SPACE_WIDTH,
        (BOARD_HEIGHT - INVISIBLE_ROWS) * SCALE + TOP_SPACE_HEIGHT,
        0
    );
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    key_state = SDL_GetKeyboardState(NULL);

    init_fonts();

    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(tick, 60, false);
    #else

        while (menu_state != Closing) {
            tick();
            SDL_Delay(1000 / 60);
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        
        SDL_Quit();

    #endif

    return 0;

}
