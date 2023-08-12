#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

#include "../include/main.h"

static TTF_Font* font;

#define RGB(r, g, b) ((r) << 24 | (g) << 16 | (b) << 8 | 0xFF)
#define GET_R(c)  (c >> 24)
#define GET_G(c)  ((c << 8) >> 24)
#define GET_B(c) ((c << 16) >> 24)
#define GET_A(c) ((c << 24) >> 24)

const Uint32 piece_colours[9] = {
    (Uint32) RGB(  0,   0,   0),
    (Uint32) RGB(255, 255,   0),
    (Uint32) RGB(255,   0, 255),
    (Uint32) RGB(  0, 255,   0),
    (Uint32) RGB(255, 128,   0),
    (Uint32) RGB(  0,   0, 255),
    (Uint32) RGB(  0, 255, 255),
    (Uint32) RGB(255,   0,   0),
    (Uint32) RGB(200, 200, 200)
};

void init_font() {
    TTF_Init();
    font = TTF_OpenFont("AldotheApache.ttf", FONT_SIZE);
}

void draw_mino(int cell_x, int cell_y, MinoType type) {

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