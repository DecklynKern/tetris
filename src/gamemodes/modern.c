#include <SDL2/SDL.h>

#include "../../include/main.h"

static const Point srs_minos[7][4][PIECE_MINO_COUNT] = {
    {   // O
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}
    },
    {   // S
        {{ 0, -1}, { 1, -1}, {-1,  0}, { 0,  0}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    },
    {   // Z
        {{-1, -1}, { 0, -1}, { 0,  0}, { 1,  0}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, {-1,  1}}
    },
    {   // L
        {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{-1, -1}, { 0, -1}, { 0,  0}, { 0,  1}}
    },
    {   // J
        {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}},
        {{-1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}}
    },
    {   // T
        {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    },
    {   // I
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
        {{-1,  1}, { 0,  1}, { 1,  1}, { 2,  1}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 0,  2}}
    }
};

static MinoType next_piece;

static Point get_kick(Rotation new_rotation, int attempt) {
    return (Point){0, 0};
}

static void on_line_clear() {
}

static MinoType generate_next_piece() {
    return L;
}

static void draw() {

    for (int y = 0; y < BOARD_HEIGHT - INVISIBLE_ROWS; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            draw_mino(x, y + 5, state.board.minos[y + INVISIBLE_ROWS][x]);
        }
    }

    if (!state.line_clear_timer && !state.are_timer) {
        for (int i = 0; i < PIECE_MINO_COUNT; i++) {
            draw_mino(state.piece.x + state.piece.minos[i].x, state.piece.y + state.piece.minos[i].y - INVISIBLE_ROWS + 5, state.piece.type);
        }
    }

    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        draw_mino(5 + srs_minos[next_piece][Rot_N][i].x, srs_minos[next_piece][Rot_N][i].y, next_piece);
    }
}

const Gamemode modern_mode = {

    .line_clear_delay = 8,
    .are_delay = 18,
    .lock_delay = 30,
    .das_delay = 12,
    .soft_drop_factor = 0,
    .gravity = 5120,
    .gravity_factor = 256,

    .can_hold = false,
    .num_kicks = 2,
    .piece_rot_minos = &srs_minos,

    .get_kick = get_kick,
    .on_line_clear = on_line_clear,
    .generate_next_piece = generate_next_piece,
    .draw = draw

};