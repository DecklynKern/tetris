#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

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
        BOARD_WIDTH * SCALE,
        (BOARD_HEIGHT - INVISIBLE_ROWS) * SCALE,
        0
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    Board board = {None};
    Piece piece;

    int line_clear_timer = 0;
    int are_timer = ARE_DELAY;
    int down_held = 0;
    int das_timer = 0;
    int das = 0;
    int das_direction = 0;
    int lock_delay_timer = LOCK_DELAY;
    int gravity_count = 0;

	int close = 0;

	while (!close) {

		SDL_Event event;

        SDL_Scancode last_pressed = 0;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {

			case SDL_QUIT:
				close = 1;
				break;

			case SDL_KEYDOWN:

                if (event.key.repeat) {
                    continue;
                }

                last_pressed = event.key.keysym.scancode;

				switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_LEFT:

                        if (!line_clear_timer && !are_timer) {
                            try_move(&board, &piece, -1, 0);
                        }

                        das_timer = DAS_DELAY;
                        das = 0;
                        das_direction = -1;

                        break;

                    case SDL_SCANCODE_RIGHT:

                        if (!line_clear_timer && !are_timer) {
                            try_move(&board, &piece, 1, 0);
                        }

                        das_timer = DAS_DELAY;
                        das = 0;
                        das_direction = 1;

                        break;

                    case SDL_SCANCODE_DOWN:
                        down_held = 1;
                        break;

                    case SDL_SCANCODE_Z:
                        if (!line_clear_timer && !are_timer) {
                            try_rotate_piece(&board, &piece, 1);
                        }
                        break;

                    case SDL_SCANCODE_X:
                        if (!line_clear_timer && !are_timer) {
                            try_rotate_piece(&board, &piece, -1);
                        }
                        break;

                    default:
                        break;
				}

            case SDL_KEYUP:

                if (event.key.repeat || event.key.keysym.scancode == last_pressed) {
                    continue;
                }

                switch (event.key.keysym.scancode) {

                    case SDL_SCANCODE_LEFT:
                        if (das_direction == -1 && (das_timer || das)) {
                            das_timer = 0;
                            das = 0;
                            das_direction = 0;
                        }
                        break;

                    case SDL_SCANCODE_RIGHT:
                        if (das_direction == 1 && (das_timer || das)) {
                            das_timer = 0;
                            das = 0;
                            das_direction = 0;
                        }
                        break;

                    case SDL_SCANCODE_DOWN:
                        down_held = 0;
                        break;

                    default:
                        break;
                }
			}
		}

        // technically missing das change restriction on frame when piece spawns
        if (das_timer && !line_clear_timer && are_timer <= ARE_DELAY - 4 && are_timer != 1) {

            das_timer--;

            if (!das_timer) {
                das = 1;
            }
        }

        if (line_clear_timer) {

            line_clear_timer--;

            if (!line_clear_timer) {
                are_timer = ARE_DELAY;
            }

        } else if (are_timer) {

            are_timer--;
        
            if (!are_timer) {

                Rotation irs_rotation;

                if (key_state[SDL_SCANCODE_Z]) {
                    irs_rotation = Rot_W;

                } else if (key_state[SDL_SCANCODE_X]) {
                    irs_rotation = Rot_E;
                    
                } else {
                    irs_rotation = Rot_N;
                }

                next_piece(&piece, irs_rotation);

                lock_delay_timer = LOCK_DELAY;
            
                if (!placement_valid(&board, &piece.minos, piece.x, piece.y)) {
                    close = 1;
                }
            }

        } else {

            if (down_held) {
                gravity_count += SOFT_DROP_FACTOR;
            }

            gravity_count += GRAVITY;

            while (gravity_count > GRAVITY_FACTOR) {

                gravity_count -= GRAVITY_FACTOR;

                if (!try_move(&board, &piece, 0, 1)) {
                    break;
                }
            }

            if (das) {
                try_move(&board, &piece, das_direction, 0);
            }

            if (placement_valid(&board, &piece.minos, piece.x, piece.y + 1)) {
                lock_delay_timer = LOCK_DELAY;

            } else {

                gravity_count = 0;

                if (!lock_delay_timer || down_held) {
                    
                    if (lock_piece(&board, &piece)) {
                        line_clear_timer = LINE_CLEAR_DELAY;

                    } else {
                        are_timer = ARE_DELAY;
                    }

                } else {
                    lock_delay_timer--;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

        for (int y = 0; y < BOARD_HEIGHT - INVISIBLE_ROWS; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                draw_mino(renderer, x, y, board.minos[y + INVISIBLE_ROWS][x]);
            }
        }

        if (!line_clear_timer && !are_timer) {
            for (int i = 0; i < PIECE_MINO_COUNT; i++) {
                draw_mino(renderer, piece.x + piece.minos[i][0], piece.y + piece.minos[i][1] - INVISIBLE_ROWS, piece.type);
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