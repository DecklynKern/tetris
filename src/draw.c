#include "../include/main.h"

static TTF_Font* font;

#define GET_R(c)  (c >> 24)
#define GET_G(c)  ((c << 8) >> 24)
#define GET_B(c) ((c << 16) >> 24)
#define GET_A(c) ((c << 24) >> 24)

void init_font() {
    TTF_Init();
    font = TTF_OpenFont("AldotheApache.ttf", FONT_SIZE);
}

void draw_mino(int cell_x, int cell_y, MinoType type, const Uint32 piece_colours[9]) {

    SDL_Rect rect = {
        .x = cell_x * SCALE,
        .y = cell_y * SCALE,
        .w = SCALE,
        .h = SCALE
    };

    Uint32 colour = piece_colours[type];

    SDL_SetRenderDrawColor(renderer, GET_R(colour), GET_G(colour), GET_B(colour), GET_A(colour));
    SDL_RenderFillRect(renderer, &rect);

}

void draw_text(int x, int y, const char* text) {

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

void draw_info_value(int row, const char* format, int value) {
    char text[20];
    sprintf(text, format, value);
    draw_text(BOARD_WIDTH * SCALE, 100 + row * 15, text);
}

void draw_info_text(int row, const char* format, char* text) {
    char final_text[20];
    sprintf(final_text, format, text);
    draw_text(BOARD_WIDTH * SCALE, 100 + row * 15, final_text);
}