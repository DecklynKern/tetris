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
        BOARD_HEIGHT * SCALE,
        0
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Board board = {None};
    Piece piece;

    int down_held = 0;
    int soft_drop_timer = 0;
    int lock_delay_timer = LOCK_DELAY;

    next_piece(&piece);

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
				case SDL_SCANCODE_A:
				case SDL_SCANCODE_LEFT:
                    try_move(&board, &piece, -1, 0);
					break;
				case SDL_SCANCODE_D:
				case SDL_SCANCODE_RIGHT:
                    try_move(&board, &piece, 1, 0);
					break;
				case SDL_SCANCODE_S:
				case SDL_SCANCODE_DOWN:
                    down_held = 1;
                    soft_drop_timer = SOFT_DROP_DELAY;
					break;
				default:
					break;
				}

            case SDL_KEYUP:

                if (event.key.repeat || event.key.keysym.scancode == last_pressed) {
                    continue;
                }

                switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down_held = 0;
                    break;
                default:
                    break;
                }
			}
		}

        if (soft_drop_timer) {
            soft_drop_timer--;

        } else if (down_held) {
            soft_drop_timer = SOFT_DROP_DELAY;
            try_move(&board, &piece, 0, 1);
        }

        if (!placement_valid(&board, &piece.minos, piece.x, piece.y + 1)) {

            if (!lock_delay_timer) {

                lock_piece(&board, &piece);
                next_piece(&piece);

                soft_drop_timer = 0;
                lock_delay_timer = LOCK_DELAY;

            } else {
                lock_delay_timer--;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderClear(renderer);

        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                draw_mino(renderer, x, y, board.minos[x][y]);
            }
        }

        for (int i = 0; i < PIECE_MINO_COUNT; i++) {
            draw_mino(renderer, piece.x + piece.minos[i][0], piece.y + piece.minos[i][1], piece.type);
        }

		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / 60);
        
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	SDL_Quit();

    return 0;

}