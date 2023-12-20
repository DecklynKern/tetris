#include <SDL2/SDL_ttf.h>

#include "../include/main.h"

#define FONT_PATH "font/AldotheApache.ttf"

static TTF_Font* small_font;
static TTF_Font* large_font;

static Uint8 get_r(Uint32 colour) {
    return colour >> 24;
}

static Uint8 get_g(Uint32 colour) {
    return ((colour << 8) >> 24);
}

static Uint8 get_b(Uint32 colour) {
    return ((colour << 16) >> 24);
}

static Uint8 get_a(Uint32 colour) {
    return ((colour << 24) >> 24);
}

static Uint32 ghostify(Uint32 colour) {
    return RGB(get_r(colour) / 2, get_g(colour) / 2, get_b(colour) / 2);
}

void init_fonts(void) {
    
    TTF_Init();

    small_font = TTF_OpenFont(FONT_PATH, SMALL_FONT_SIZE);
    large_font = TTF_OpenFont(FONT_PATH, LARGE_FONT_SIZE);

}

void draw_mino_as_colour(int cell_x, int cell_y, int scale, Uint32 colour) {

    SDL_Rect rect = {
        .x = cell_x * scale,
        .y = (cell_y - INVISIBLE_ROWS) * scale + TOP_SPACE_HEIGHT,
        .w = scale,
        .h = scale
    };

    SDL_SetRenderDrawColor(renderer, get_r(colour), get_g(colour), get_b(colour), get_a(colour));
    SDL_RenderFillRect(renderer, &rect);
    
}

void draw_mino_scaled(int cell_x, int cell_y, MinoType type, int scale) {
    draw_mino_as_colour(cell_x, cell_y, scale, (*state.gamemode.piece_colours)[type]);
}

void draw_mino(int cell_x, int cell_y, MinoType type) {
    draw_mino_scaled(cell_x, cell_y, type, SCALE);
}

void draw_text(int x, int y, const char* text, TTF_Font* font) {

    SDL_Colour white = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, white);

    if (!surface) {
        return;
    }

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

void draw_large_text_centered_x(int y, const char *text) {
    int width, height;
    TTF_SizeText(large_font, text, &width, &height);
    draw_large_text(SCREEN_CENTRE_X - width / 2, y, text);
}

void draw_info_value(int row, const char* format, int value) {
    char text[20];
    sprintf(text, format, value);
    draw_small_text(BOARD_WIDTH * SCALE, 20 + row * 15, text);
}

void draw_info_text(int row, const char* format, const char* text) {
    char final_text[20];
    sprintf(final_text, format, text);
    draw_small_text(BOARD_WIDTH * SCALE, 20 + row * 15, final_text);
}

void draw_info_timer(int row) {
    char text[30];
    get_timer_formatted(text);
    draw_small_text(BOARD_WIDTH * SCALE, 20 + row * 15, text);
}

void draw_piece_north(MinoType type, int offset_x, int offset_y) {

    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        draw_mino(
            (*state.gamemode.piece_rot_minos)[type - 1][Rot_N][i].x + offset_x,
            (*state.gamemode.piece_rot_minos)[type - 1][Rot_N][i].y + INVISIBLE_ROWS + offset_y,
            type
        );
    }
}

void draw_board(void) {

    for (int y = INVISIBLE_ROWS; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            draw_mino(x, y, board[y][x]);
        }
    }

    if (get_held_piece() != Piece_Empty) {
        draw_piece_north(get_held_piece(), 1, -3);
    }

    if (!piece_active()) {
        return;
    }
        
    if (state.gamemode.show_ghost) {

        int ghost_y = state.piece.y;

        while (placement_valid(get_piece_minos(), state.piece.x, ghost_y)) {
            ghost_y++;
        }

        ghost_y--;

        for (int i = 0; i < PIECE_MINO_COUNT; i++) {
            draw_mino_as_colour(
                state.piece.x + get_piece_minos()[i].x,
                ghost_y + get_piece_minos()[i].y,
                SCALE,
                ghostify((*state.gamemode.piece_colours)[state.piece.type])
            );
        }
    }

    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        draw_mino(
            state.piece.x + get_piece_minos()[i].x,
            state.piece.y + get_piece_minos()[i].y,
            state.piece.type
        );
    }
}

void draw_next(MinoType* next, int piece_count) {

    for (int i = 0; i < piece_count; i++) {
        draw_piece_north(next[i], 12, 4 * i - 2);
    }
}
