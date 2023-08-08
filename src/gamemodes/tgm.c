#include <SDL2/SDL.h>

#include "../../include/main.h"

#define HISTORY_LEN 4
#define NUM_TRIES 4

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
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}},
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
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
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}}
    }
};

#define NUM_SPEED_LEVELS 30
const int gravity_table[NUM_SPEED_LEVELS][2] = {
    {  0,    4},
    { 30,    6},
    { 35,    8},
    { 40,   10},
    { 50,   12},
    { 60,   16},
    { 70,   32},
    { 80,   48},
    { 90,   64},
    {100,   80},
    {120,   96},
    {140,  112},
    {160,  128},
    {170,  144},
    {200,    4},
    {220,   32},
    {230,   64},
    {233,   96},
    {236,  128},
    {239,  160},
    {243,  192},
    {247,  224},
    {251,  256},
    {300,  512},
    {330,  768},
    {360, 1024},
    {400, 1280},
    {420, 1024},
    {450,  768},
    {500, 5120},
};

const int spin_kicks[8][MAX_KICKS][2];

static MinoType next_piece;
static MinoType history[HISTORY_LEN];
static int level = 0;

static void tgm1_init() {

    history[0] = history[1] = history[2] = history[3] = Z;

    next_piece = rand() % 4 + 4;
    history[0] = next_piece;

}

static void tgm2_init() {

    history[0] = history[3] = Z;
    history[1] = history[2] = S;

    next_piece = rand() % 4 + 4;
    history[0] = next_piece;

}

static Point get_kick(Rotation new_rotation, int attempt) {

    if (state.piece.type == I) {
        return (Point){0, 0};
    }

    if (state.piece.type == L || state.piece.type == J || state.piece.type == T) {

        for (int dy = -1; dy < 2; dy++) {
            for (int dx = -1; dx < 2; dx++) {

                if (state.board.minos[state.piece.y + dy][state.piece.x + dx]) {

                    if (dx == 0) {
                        return (Point){0, 0};
                    }

                    goto no_centre_column;

                }
            }
        }
    }
    no_centre_column:
    
    if (attempt == 1){
        return (Point){1, 0};
    }
        
    return (Point){-1, 0};

}

static void on_lock() {
    if (level % 100 != 99 && level != 998) {
        level++;
    }
}

static void on_line_clear(int num_lines) {

    level += num_lines;

    int i = 0;

    for (; i < NUM_SPEED_LEVELS; i++) {
        if (gravity_table[i][0] > level) {
            i--;
            break;
        }
    }

    state.gamemode.gravity = gravity_table[i][1];

}

static MinoType generate_next_piece() {

    MinoType try_piece = rand() % 7 + 1;

    for (int try = 0; try < NUM_TRIES; try++) {

        bool no_conflict = true;

        for (int i = 0; i < HISTORY_LEN; i++) {

            if (history[i] == try_piece) {
                no_conflict = false;
                break;
            }
        }

        if (no_conflict) {
            break;
        }

        try_piece = rand() % 7 + 1;

    }

    for (int i = 1; i < HISTORY_LEN; i++) {
        history[i] = history[i - 1];
    }

    history[0] = try_piece;

    if (state.input_held[Input_Rot_CCW]) {
        state.piece.rotation = Rot_W;

    } else if (state.input_held[Input_Rot_CW]) {
        state.piece.rotation = Rot_E;
        
    } else {
        state.piece.rotation = Rot_N;
    }

    MinoType new_piece_type = next_piece;
    next_piece = try_piece;

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

    char level_text[15];
    sprintf(level_text, "Level: %d", level);
    draw_text(renderer, BOARD_WIDTH * SCALE, 100, level_text);

}

const Gamemode tgm1_mode = {

    .screen_width = BOARD_WIDTH * SCALE + 5 * SCALE,
    .screen_height = (BOARD_HEIGHT - INVISIBLE_ROWS) * SCALE + 5 * SCALE,

    .line_clear_delay = 41,
    .are_delay = 30,
    .lock_delay = 30,
    .gravity = 4,
    .das_delay = 16,
    .soft_drop_factor = 256,

    .can_hold = false,
    .num_kicks = 2,
    .piece_rot_minos = &piece_rot_minos,

    .init = &tgm1_init,
    .get_kick = &get_kick,
    .on_lock = &on_lock,
    .on_line_clear = &on_line_clear,
    .generate_next_piece = &generate_next_piece,
    .draw = &draw

};

const Gamemode tap_death_mode = {

    .screen_width = BOARD_WIDTH * SCALE + 5 * SCALE,
    .screen_height = (BOARD_HEIGHT - INVISIBLE_ROWS) * SCALE + 5 * SCALE,

    .line_clear_delay = 8,
    .are_delay = 18,
    .lock_delay = 30,
    .gravity = 5120,
    .das_delay = 12,
    .soft_drop_factor = 256,

    .can_hold = false,
    .num_kicks = 2,
    .piece_rot_minos = &piece_rot_minos,

    .init = &tgm2_init,
    .get_kick = &get_kick,
    .on_lock = &on_lock,
    .on_line_clear = &on_line_clear,
    .generate_next_piece = &generate_next_piece,
    .draw = &draw

};