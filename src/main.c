#include <SDL2/SDL_events.h>
#include <SDL2/SDL_scancode.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define SDL_FLAGS SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC | SDL_INIT_AUDIO)
#else
    #define SDL_FLAGS SDL_INIT_EVERYTHING
#endif

#include "../include/main.h"
#include "../include/menu.h"

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
static SDL_Window* window;
SDL_Renderer* renderer;
 
static const Uint8* key_state;

static bool in_menu(void) {
    return current_menu;
}

static bool in_game(void) {
    return !current_menu;
}

static void do_quit(void) {

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    exit(0);
    
}

static void tick(void) {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {

        case SDL_QUIT:
            do_quit();
            break;

        case SDL_KEYDOWN:

            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                
                if (current_menu == &pause_menu) {
                    current_menu = NULL;
                    timer_unpause();
                }
                else if (in_game()) {
                    load_menu(&pause_menu);
                    timer_pause();
                }

                continue;

            }

            if (in_menu()) {
                
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_UP:
                        menu_move_up();
                        break;
                    case SDL_SCANCODE_DOWN:
                        menu_move_down();
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_SPACE:
                        menu_select();
                        break;
                    default:
                        menu_handle_key(event.key.keysym.scancode);
                }
                
                continue;
                
            }

            if (event.key.repeat) {
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
            else if (event.key.keysym.scancode == mapped_keys[Input_Rot_CCW]) {
                input_rotate_ccw();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_Rot_CW]) {
                input_rotate_cw();
            }
            else if (event.key.keysym.scancode == mapped_keys[Input_Hold]) {
                input_hold();
            }

            break;

        case SDL_KEYUP:

            if (in_menu() || event.key.repeat) {
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

    for (int key = 0; key < NUM_HOLDABLE_KEYS; key++) {
        state.input_held[key] = key_state[mapped_keys[key]];
    }

    if (in_game()) {

        state.timer_ms = get_timer_seconds() * 1000;

        update();
        
        if (state.quit_to_main_menu) {
            load_menu(&main_menu);
        }
    }

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
    
    if (in_game()) {
        state.gamemode.draw();
    }
    else {
        draw_menu();
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
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );

    load_menu(&main_menu);
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    key_state = SDL_GetKeyboardState(NULL);

    init_fonts();

    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(tick, 60, false);
    #else

        while (1) {
            tick();
            SDL_Delay((int) round(1000.0 / (state.gamemode.fps == 0 ? 60 : state.gamemode.fps)));
        }

    #endif

    return 0;

}
