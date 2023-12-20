#include <time.h>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #define SDL_FLAGS SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC | SDL_INIT_AUDIO)
#else
    #define SDL_FLAGS SDL_INIT_EVERYTHING
#endif

#include "../include/menu.h"
#include "../include/input.h"

GameData state = {0};
static SDL_Window* window;
SDL_Renderer* renderer;
 
static const Uint8* key_state;

static bool exit_game = false;

void set_exit_game(void) {
    exit_game = true;
}

void do_quit(void) {

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
            handle_keydown(event.key.keysym.scancode, event.key.repeat);
            break;

        case SDL_KEYUP:
            handle_keyup(event.key.keysym.scancode);
            break;

        }
    }

    for (int key = 0; key < NUM_HOLDABLE_KEYS; key++) {
        input_held[key] = key_state[mapped_keys[key]];
    }

    if (!current_menu) {

        update();
        
        if (exit_game) {

            exit_game = false;
            
            memset(state.result.line1, 0, sizeof(state.result.line1));
            memset(state.result.line2, 0, sizeof(state.result.line2));
            memset(state.result.line3, 0, sizeof(state.result.line3));
            
            TRY_CALL(state.gamemode.on_exit);
            load_menu(&results_menu);
            
        }
    }

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
    
    if (!current_menu) {
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
