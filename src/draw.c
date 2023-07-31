#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "../include/main.h"

const Uint32 piece_colours[8] = {
    (Uint32) RGB(  0,   0,   0),
    (Uint32) RGB(255, 128,   0),
    (Uint32) RGB(  0,   0, 255),
    (Uint32) RGB(255, 255,   0),
    (Uint32) RGB(255,   0, 255),
    (Uint32) RGB(  0, 255,   0),
    (Uint32) RGB(  0, 255, 255),
    (Uint32) RGB(255,   0,   0)
};

void draw_mino(SDL_Renderer* renderer, int x, int y, MinoType type) {

    SDL_Rect rect = {
        .x = x * SCALE,
        .y = y * SCALE,
        .w = SCALE,
        .h = SCALE
    };

    Uint32 colour = piece_colours[type];

    SDL_SetRenderDrawColor(renderer, GET_R(colour), GET_G(colour), GET_B(colour), GET_A(colour));

    SDL_RenderFillRect(renderer, &rect);

}