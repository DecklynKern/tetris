#include "../include/main.h"
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define FONT_PATH "font/AldotheApache.ttf"

static TTF_Font* small_font;
static TTF_Font* large_font;

#define GET_R(c)  (c >> 24)
#define GET_G(c)  ((c << 8) >> 24)
#define GET_B(c) ((c << 16) >> 24)
#define GET_A(c) ((c << 24) >> 24)
#define GHOSTIFY(c) RGB(GET_R(c) / 2, GET_G(c) / 2, GET_B(c) / 2)

void init_fonts(void) {
    
    TTF_Init();

    small_font = TTF_OpenFont(FONT_PATH, SMALL_FONT_SIZE);
    large_font = TTF_OpenFont(FONT_PATH, LARGE_FONT_SIZE);

}

void draw_mino_as_colour(int cell_x, int cell_y, int scale, Uint32 colour) {

    SDL_Rect rect = {
        .x = cell_x * scale,
        .y = cell_y * scale,
        .w = scale,
        .h = scale
    };

    SDL_SetRenderDrawColor(renderer, GET_R(colour), GET_G(colour), GET_B(colour), GET_A(colour));
    SDL_RenderFillRect(renderer, &rect);
    
}

void draw_mino_scaled(int cell_x, int cell_y, MinoType type, int scale) {
    draw_mino_as_colour(cell_x, cell_y, scale, (*state.gamemode.piece_colours)[type]);
}

void draw_mino(int cell_x, int cell_y, MinoType type) {
    draw_mino_scaled(cell_x, cell_y, type, SCALE);
}

void draw_text(int x, int y, const char* text, const TTF_Font* font) {

    SDL_Colour white = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, white);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {
        x,
        y,
        surface->w,
        surface->h
    };

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

}

void draw_small_text(int x, int y, const char* text) {
    draw_text(x, y, text, small_font);
}

void draw_large_text(int x, int y, const char* text) {
    draw_text(x, y, text, large_font);
}

void draw_info_value(int row, const char* format, int value) {
    char text[20];
    sprintf(text, format, value);
    draw_small_text(BOARD_WIDTH * SCALE, 100 + row * 15, text);
}

void draw_info_text(int row, const char* format, const char* text) {
    char final_text[20];
    sprintf(final_text, format, text);
    draw_small_text(BOARD_WIDTH * SCALE, 100 + row * 15, final_text);
}

void draw_info_timer(int row) {
    char text[30];
    sprintf(text, "%02ld:%02ld.%03ld", state.timer_ms / 60000, state.timer_ms / 1000 % 60, state.timer_ms % 1000);
    draw_small_text(BOARD_WIDTH * SCALE, 100 + row * 15, text);
}

void draw_board() {

    for (int y = 0; y < BOARD_HEIGHT - INVISIBLE_ROWS; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            draw_mino(x, y + 5, state.board.minos[y + INVISIBLE_ROWS][x]);
        }
    }

    if (!state.line_clear_timer && !state.are_timer) {

        if (state.gamemode.show_ghost) {

            int ghost_y = state.piece.y;

            while (placement_valid(get_piece_minos(), state.piece.x, ghost_y)) {
                ghost_y++;
            }

            ghost_y--;

            for (int i = 0; i < PIECE_MINO_COUNT; i++) {
                draw_mino_as_colour(
                    state.piece.x + get_piece_minos()[i].x,
                    ghost_y + get_piece_minos()[i].y - INVISIBLE_ROWS + 5,
                    SCALE,
                    GHOSTIFY((*state.gamemode.piece_colours)[state.piece.type])
                );
            }
        }

        for (int i = 0; i < PIECE_MINO_COUNT; i++) {
            draw_mino(
                state.piece.x + get_piece_minos()[i].x,
                state.piece.y + get_piece_minos()[i].y - INVISIBLE_ROWS + 5,
                state.piece.type
            );
        }
    }
}

void draw_single_next(MinoType next) {

    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        draw_mino(
            (*state.gamemode.piece_rot_minos)[next - 1][Rot_N][i].x + 5,
            (*state.gamemode.piece_rot_minos)[next - 1][Rot_N][i].y + 1,
            next
        );
    }
}
