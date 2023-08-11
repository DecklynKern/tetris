#include <SDL2/SDL.h>

#include "../../include/main.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

static const Point piece_rot_minos[8][4][PIECE_MINO_COUNT] = {
    {},
    {   // O
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}
    },
    {   // S
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}}
    },
    {   // Z
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}}
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
        {{-1,  1}, { 0,  1}, { 1,  1}, { 2,  1}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
        {{-1,  1}, { 0,  1}, { 1,  1}, { 2,  1}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}}
    }
};

static MinoType next_piece;

static int start_level = 0;
static int level = 0;
static int lines = 0;
static int score = 0;

#define NUM_LEVELS 30

static const int gravity_factor_table[NUM_LEVELS] = {
    48,
    43,
    38,
    33,
    28,
    23,
    18,
    13,
    8,
    6,
    5,
    5,
    5,
    4,
    4,
    4,
    3,
    3,
    3,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    1
};

static void init() {
    next_piece = rand() % 7 + 1;
    state.gamemode.gravity_factor = gravity_factor_table[level];
    state.gamemode.soft_drop_factor = state.gamemode.gravity_factor / 2;
}

static void on_lock() {
}

static void on_line_clear(int num_lines) {

    lines += lines;

    if ((level != start_level && lines >= 10) ||
        (level == start_level && (
            lines >= start_level * 10 + 10 ||
            lines >= MAX(100, start_level * 10 - 50)))
    ) {

        level++;
        lines -= 10;

        state.gamemode.gravity_factor = gravity_factor_table[level];
        state.gamemode.soft_drop_factor = state.gamemode.gravity_factor / 2;

    }

    switch (num_lines) {

        case 0:
            score += 40 * (level + 1);
            break;

        case 1:
            score += 100 * (level + 1);
            break;

        case 2:
            score += 300 * (level + 1);
            break;

        case 3:
            score += 1200 * (level + 1);
            break;

    }
}

static MinoType new_piece() {

    MinoType new_piece_type = next_piece;
    next_piece = rand() % 7 + 1;
    
    return new_piece_type;

}

static void draw(SDL_Renderer* renderer) {

    for (int y = 0; y < BOARD_HEIGHT - INVISIBLE_ROWS; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            draw_mino(renderer, x, y + 5, state.board.minos[y + INVISIBLE_ROWS][x]);
        }
    }

    if (!state.line_clear_timer && !state.are_timer) {
        for (int i = 0; i < PIECE_MINO_COUNT; i++) {
            draw_mino(renderer, state.piece.x + state.piece.minos[i].x, state.piece.y + state.piece.minos[i].y - INVISIBLE_ROWS + 5, state.piece.type);
        }
    }

    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        draw_mino(renderer, 5 + piece_rot_minos[next_piece][Rot_N][i].x, piece_rot_minos[next_piece][Rot_N][i].y, next_piece);
    }

    draw_info_text(renderer, 0, "Level: %d", level);
    draw_info_text(renderer, 1, "Lines: %d", lines);
    draw_info_text(renderer, 2, "Score: %d", score);

}

const Gamemode nes_mode = {

    .screen_width = BOARD_WIDTH * SCALE + 5 * SCALE,
    .screen_height = (BOARD_HEIGHT - INVISIBLE_ROWS) * SCALE + 5 * SCALE,

    .line_clear_delay = 18, // 17-20 depending on internal frame counter???
    .are_delay = 10, // 10-18 depending on height at lock
    .gravity = 1,
    .lock_delay = 0,
    .das_delay = 16,

    .can_hold = false,
    .num_kicks = 0,
    .piece_rot_minos = &piece_rot_minos,

    .init = init,
    .on_lock = on_lock,
    .on_line_clear = on_line_clear,
    .generate_next_piece = new_piece,
    .draw = draw

};