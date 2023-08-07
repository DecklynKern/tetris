#include <stdio.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_video.h>

#include "../include/main.h"

int main() {

    srand(time(NULL));

    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("sdl bruh: %s\n", SDL_GetError());
    }

    SDL_Window* window = SDL_CreateWindow(
        "Tetris!",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        BOARD_WIDTH * SCALE + 5 * SCALE,
        (BOARD_HEIGHT - INVISIBLE_ROWS) * SCALE,
        0
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    GameData state = {0};
    memcpy(&state.gamemode, &tgm1_mode, sizeof(Gamemode));

	bool close = false;

	while (!close) {

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
            switch (event.type) {

			case SDL_QUIT:
                close = true;
                break;

            case SDL_KEYDOWN:

                if (event.key.repeat) {
                    continue;
                }

                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_LEFT:
                        input_left(&state);
                        break;

                    case SDL_SCANCODE_RIGHT:
                        input_right(&state);
                        break;

                    case SDL_SCANCODE_Z:
                        input_rotate_ccw(&state);
                        break;

                    case SDL_SCANCODE_X:
                        input_rotate_cw(&state);
                        break;

                    default:
                        break;
				}

                break;

            case SDL_KEYUP:

                if (event.key.repeat) {
                    continue;
                }

                switch (event.key.keysym.scancode) {

                    case SDL_SCANCODE_LEFT:
                        release_left(&state);
                        break;

                    case SDL_SCANCODE_RIGHT:
                        release_right(&state);
                        break;

                    default:
                        break;
                }
			}
		}

        // for (int key = 0; key < _Num_Holdable_Keys; key++) {
        //     state.held_inputs[key] = ;
        // }

        state.held_inputs[Input_Left] = key_state[SDL_SCANCODE_LEFT];
        state.held_inputs[Input_Right] = key_state[SDL_SCANCODE_RIGHT];
        state.held_inputs[Input_Down] = key_state[SDL_SCANCODE_DOWN];
        state.held_inputs[Input_Rot_CW] = key_state[SDL_SCANCODE_Z];
        state.held_inputs[Input_Rot_CCW] = key_state[SDL_SCANCODE_X];

        close |= update(&state);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

        for (int y = 0; y < BOARD_HEIGHT - INVISIBLE_ROWS; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                draw_mino(renderer, x, y, state.board.minos[y + INVISIBLE_ROWS][x]);
            }
        }

        if (!state.line_clear_timer && !state.are_timer) {
            for (int i = 0; i < PIECE_MINO_COUNT; i++) {
                draw_mino(renderer, state.piece.x + state.piece.minos[i].x, state.piece.y + state.piece.minos[i].y - INVISIBLE_ROWS, state.piece.type);
            }
        }

		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / 60);
        
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	SDL_Quit();

    return 0;

}