#include "../../include/main.h"
#include <SDL2/SDL_stdinc.h>

// much of this is taken from the fantastic article at
// https://meatfighter.com/nintendotetrisai/?a=b#The_Mechanics_of_Nintendo_Tetris

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

static const int nes_colour_table[120][4] = {
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
    0x0F, 0x30, 0x21, 0x12,
    0x0F, 0x30, 0x29, 0x1A,
    0x0F, 0x30, 0x24, 0x14,
    0x0F, 0x30, 0x2A, 0x12,
    0x0F, 0x30, 0x2B, 0x15,
    0x0F, 0x30, 0x22, 0x2B,
    0x0F, 0x30, 0x00, 0x16,
    0x0F, 0x30, 0x05, 0x13,
    0x0F, 0x30, 0x16, 0x12,
    0x0F, 0x30, 0x27, 0x16,
};

static const Uint32 nes_colour_palette[64] = {
    RGB(124, 124, 124),
    RGB(  0,   0, 252),
    RGB(  0,   0, 188),
    RGB( 68,  40, 188),
    RGB(148,   0, 132),
    RGB(168,   0,  32),
    RGB(168,  16,   0),
    RGB(136,  20,   0),
    RGB( 80,  48,   0),
    RGB(  0, 120,   0),
    RGB(  0, 104,   0),
    RGB(  0,  88,   0),
    RGB(  0,  64,  88),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(188, 188, 188),
    RGB(  0, 120, 248),
    RGB(  0,  88, 248),
    RGB(104,  68, 252),
    RGB(216,   0, 204),
    RGB(228,   0,  88),
    RGB(248,  56,   0),
    RGB(228,  92,  16),
    RGB(172, 124,   0),
    RGB(  0, 184,   0),
    RGB(  0, 168,   0),
    RGB(  0, 168,  68),
    RGB(  0, 136, 136),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(248, 248, 248),
    RGB( 60, 188, 252),
    RGB(104, 136, 252),
    RGB(152, 120, 248),
    RGB(248, 120, 248),
    RGB(248,  88, 152),
    RGB(248, 120,  88),
    RGB(252, 160,  68),
    RGB(248, 184,   0),
    RGB(184, 248,  24),
    RGB( 88, 216,  84),
    RGB( 88, 248, 152),
    RGB(  0, 232, 216),
    RGB(120, 120, 120),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0),
    RGB(252, 252, 252),
    RGB(164, 228, 252),
    RGB(184, 184, 248),
    RGB(216, 184, 248),
    RGB(248, 184, 248),
    RGB(248, 164, 192),
    RGB(240, 208, 176),
    RGB(252, 224, 168),
    RGB(248, 216, 120),
    RGB(216, 248, 120),
    RGB(184, 248, 184),
    RGB(184, 248, 216),
    RGB(  0, 252, 252),
    RGB(248, 216, 248),
    RGB(  0,   0,   0),
    RGB(  0,   0,   0)
};

static const Uint32 piece_colours[9] = {
    RGB(  0,   0,   0),
    RGB(128, 128, 128),
    RGB(128, 128, 128),
    RGB(128, 128, 128),
    RGB(128, 128, 128),
    RGB(128, 128, 128),
    RGB(128, 128, 128),
    RGB(128, 128, 128),
    RGB(200, 200, 200)
};

static const Point nrs_right_handed_minos[7][4][PIECE_MINO_COUNT] = {
    {   // O
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}
    }, { // S
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}}
    }, { // Z
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}}
    }, { // L
        {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{-1, -1}, { 0, -1}, { 0,  0}, { 0,  1}}
    }, { // J
        {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}},
        {{-1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}}
    }, { // T
        {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    }, { // I
        {{-1,  1}, { 0,  1}, { 1,  1}, { 2,  1}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
        {{-1,  1}, { 0,  1}, { 1,  1}, { 2,  1}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}}
    }
};

static const Point nrs_left_handed_minos[7][4][PIECE_MINO_COUNT] = {
    {   // O
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}
    }, { // S
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}},
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    }, { // Z
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, {-1,  1}},
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, {-1,  1}}
    }, { // L
        {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{-1, -1}, { 0, -1}, { 0,  0}, { 0,  1}}
    }, { // J
        {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}},
        {{-1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}}
    }, { // T
        {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    }, { // I
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}}
    }
};


static int start_level = 0;
static int level = 0;
static int lines = 0;
static int score = 0;

static MinoType next_piece;

static Uint8 total_pieces = 0;
static Uint8 last_spawned_mino_id = 0;
static Uint16 rng_seed = 0x8988;
static Uint8 nes_to_tetrism_minotype[7] = {T, J, Z, O, S, L, I};
static Uint8 nes_orientation_id_table[7] = {2, 7, 8, 10, 11, 14, 18};

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

static void update_lfsr(void);
static MinoType new_piece(void);

static void init(void) {

    // simulate random number of frames since game was turned on
    int num_frames = rand() % 100;
    for (int i = 0; i < num_frames; i++) {
        update_lfsr();
    }
    
    next_piece = new_piece();

    state.gamemode.gravity_factor = gravity_factor_table[level];
    state.gamemode.soft_drop_factor = state.gamemode.gravity_factor / 2;

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

static void update_lfsr(void) {
    rng_seed = ((((rng_seed >> 9) & 1) ^ ((rng_seed >> 1) & 1)) << 15) | (rng_seed >> 1);
}

static MinoType new_piece(void) {

    total_pieces++;

    Uint8 index = rng_seed >> 8;
    index += total_pieces;
    index &= 7;

    if (index == 7 || index == last_spawned_mino_id) {

        update_lfsr();

        index = rng_seed >> 8;
        index &= 7;
        index += nes_orientation_id_table[last_spawned_mino_id];
        index %= 7;

    }

    last_spawned_mino_id = index;

    MinoType to_spawn = next_piece;
    next_piece = nes_to_tetrism_minotype[index];
    return to_spawn;

}

static void nes_update(void) {
    update_lfsr();
}

static void draw(void) {

    draw_board();
    draw_single_next(next_piece);

    draw_info_value(0, "Level: %d", level);
    draw_info_value(1, "Lines: %d", lines);
    draw_info_value(2, "Score: %d", score);

}

const Gamemode nes_mode = {

    .line_clear_delay = 18, // 17-20 depending on internal frame counter???
    .are_delay = 10, // 10-18 depending on height at lock
    .lock_delay = 0,
    .das_delay = 16,
    .arr_delay = 6,
    .gravity = 1,

    .show_ghost = false,
    .can_hold = false,
    .lock_on_down_held = false,
    .irs = false,
    .instant_drop_type = NoInstantDrop,
    .num_kicks = 0,
    .piece_rot_minos = &nrs_right_handed_minos,
    .piece_colours = (Uint32 (*const)[]) &piece_colours,

    .init = init,
    .on_line_clear = on_line_clear,
    .generate_new_piece = new_piece,
    .update = nes_update,
    .draw = draw

};

// unfinished
// const Gamemode gba_mode = {

//     .line_clear_delay = 91,
//     .are_delay = 2,
//     .lock_delay = 0,
//     .das_delay = 23,
//     .arr_delay = 9,
//     .gravity = 1,

//     .can_hold = false,
//     .num_kicks = 0,
//     .piece_rot_minos = &nrs_left_handed_minos,

//     .init = init,
//     .on_line_clear = on_line_clear,
//     .generate_new_piece = new_piece,
//     .draw = draw

// };
