#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "../include/main.h"

void draw_mino(SDL_Renderer* renderer, int x, int y, MinoType type) {

    SDL_Rect rect = {
        .x = x * SCALE,
        .y = y * SCALE,
        .w = SCALE,
        .h = SCALE
    };

    Uint32 colour;

    switch (type) {
        case None:
            colour = COLOUR_NONE;
            break;

        case L:
            colour = COLOUR_L;
            break;

        case J:
            colour = COLOUR_J;
            break;

        case O:
            colour = COLOUR_O;
            break;

        case S:
            colour = COLOUR_S;
            break;

        case Z:
            colour = COLOUR_Z;
            break;

        case T:
            colour = COLOUR_T;
            break;

        case I:
            colour = COLOUR_I;
            break;
    }

    SDL_SetRenderDrawColor(renderer, GET_R(colour), GET_G(colour), GET_B(colour), GET_A(colour));

    SDL_RenderFillRect(renderer, &rect);

}