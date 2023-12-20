#include <SDL2/SDL_stdinc.h>

#include "../../include/classic.h"
#include "../../include/menu.h"

// much of this is taken from the fantastic article at
// https://meatfighter.com/nintendotetrisai/?a=b#The_Mechanics_of_Nintendo_Tetris

static int start_level = 0;
static int type_b_garbage_level = 0;
static int level;
static int score;

static MinoType next_piece;

static Uint8 total_pieces;
static Uint8 last_spawned_mino_id;
static Uint16 rng_seed;

static void nes_init(void) {
    
    level = start_level;
    score = 0;
    total_pieces = 0;
    last_spawned_mino_id = 0;
    rng_seed = 0x8988;

    // simulate random number of frames since game was turned on
    int num_frames = rand() % 250;
    for (int i = 0; i < num_frames; i++) {
        update_lfsr();
    }
    
    new_piece();

    state.gamemode.gravity_factor = gravity_factor_table[level];
    state.gamemode.soft_drop_factor = state.gamemode.gravity_factor / 2;
    update_colours();

}

static void nes_type_b_init(void) {

    nes_init();

    for (int y = 0; y < 2 * type_b_garbage_level; y++) {
        for (int x = 0; x < 10; x++) {

            if (rand() % 2) {
                board[BOARD_HEIGHT - y - 1][x] = Piece_Garbage;
            }
        }
    }
}

static void nes_on_line_clear(int num_lines) {

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

static void nes_type_a_on_line_clear(int num_lines) {

    int lines_cleared = get_lines_cleared();

    if ((level != start_level && lines_cleared % 10 < (lines_cleared - num_lines) % 10) ||
        (level == start_level && (
            lines_cleared >= start_level * 10 + 10 ||
            lines_cleared >= MAX(100, start_level * 10 - 50)))
    ) {

        level++;

        state.gamemode.gravity_factor = gravity_factor_table[level];
        state.gamemode.soft_drop_factor = state.gamemode.gravity_factor / 2;
        update_colours();

    }

    nes_on_line_clear(num_lines);
    
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

static void update_colours(void) {
    piece_colours[Piece_T] = nes_colour_palette[nes_level_colours[level][1]];
    piece_colours[Piece_J] = nes_colour_palette[nes_level_colours[level][2]];
    piece_colours[Piece_Z] = nes_colour_palette[nes_level_colours[level][3]];
    piece_colours[Piece_O] = nes_colour_palette[nes_level_colours[level][1]];
    piece_colours[Piece_S] = nes_colour_palette[nes_level_colours[level][2]];
    piece_colours[Piece_L] = nes_colour_palette[nes_level_colours[level][3]];
    piece_colours[Piece_I] = nes_colour_palette[nes_level_colours[level][1]];
}

static void nes_update(void) {
    update_lfsr();
}

static void draw(void) {

    draw_board();
    draw_next(&next_piece, 1);

    draw_info_value(0, "Level: %d", level);
    draw_info_value(1, "Lines: %d", get_lines_cleared());
    draw_info_value(2, "Score: %d", score);

}

static void nes_type_a_on_exit(void) {
    sprintf(state.result.line1, "Score: %d", score);
    sprintf(state.result.line2, "Level: %d", level);
    sprintf(state.result.line3, "Lines: %d", get_lines_cleared());
}

static void nes_type_b_on_exit(void) {
    sprintf(state.result.line1, lines_cleared_at(25) ? "Success" : "Fail");
    get_timer_formatted(state.result.line2);
}

// line clear delay 17-20 depending on internal frame counter???
// are delay 10-18 depending on height at lock?
#define NES_SETTINGS \
    \
    .line_clear_delay = 18, \
    .are_delay = 10, \
    .lock_delay = 0, \
    .das_delay = 16, \
    .arr_delay = 6, \
    .gravity = 1, \
    \
    .show_ghost = false, \
    .can_hold = false, \
    .lock_on_down_held = false, \
    .irs = false, \
    .instant_drop_type = Drop_NoInstant, \
    .socd_allow_das_overwrite = SOCD_Right, \
    .num_kicks = 0, \
    .piece_rot_minos = &nrs_right_handed_minos, \
    .piece_colours = (Uint32 (*const)[]) &piece_colours, \
    \
    .generate_new_piece = new_piece, \
    .update = nes_update, \
    .draw = draw

const Gamemode nes_type_a_mode = {

    NES_SETTINGS,

    .init = nes_init,
    .on_line_clear = nes_type_a_on_line_clear,
    .on_exit = nes_type_a_on_exit

};

const Gamemode nes_type_b_mode = {

    NES_SETTINGS,

    .max_lines = 25,

    .init = nes_type_b_init,
    .on_line_clear = nes_on_line_clear,
    .on_exit = nes_type_b_on_exit

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

const Menu nes_menu;
const Menu nes_type_a_menu = MENU(
    BUTTON_NEW_MENU("Back", nes_menu),
    SEPARATOR,
    NUM_BOX("Speed Level:", 0, 19, start_level),
    BUTTON_LOAD_GAMEMODE("Start", nes_type_a_mode)
);

const Menu nes_type_b_menu = MENU(
    BUTTON_NEW_MENU("Back", nes_menu),
    SEPARATOR,
    NUM_BOX("Speed Level:", 0, 19, start_level),
    NUM_BOX("Garbage Level:", 0, 5, type_b_garbage_level),
    BUTTON_LOAD_GAMEMODE("Start", nes_type_b_mode)
);

const Menu nes_menu = MENU(
    BUTTON_NEW_MENU("Back", main_menu),
    SEPARATOR,
    BUTTON_NEW_MENU("Type A", nes_type_a_menu),
    BUTTON_NEW_MENU("Type B", nes_type_b_menu)
);
