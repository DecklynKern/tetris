#include <SDL2/SDL_stdinc.h>

#include "../../include/classic.h"
#include "../../include/menu.h"

// much of this is taken from the fantastic article at
// https://meatfighter.com/nintendotetrisai/?a=b#The_Mechanics_of_Nintendo_Tetris

static int start_level = 0;
static int level = 0;
static int lines = 0;
static int score = 0;

static MinoType next_piece;

static Uint8 total_pieces = 0;
static Uint8 last_spawned_mino_id = 0;
static Uint16 rng_seed = 0x8988;

static void init(void) {
    
    level = start_level;

    // simulate random number of frames since game was turned on
    int num_frames = rand() % 100;
    for (int i = 0; i < num_frames; i++) {
        update_lfsr();
    }
    
    new_piece();

    state.gamemode.gravity_factor = gravity_factor_table[level];
    state.gamemode.soft_drop_factor = state.gamemode.gravity_factor / 2;
    update_colours();

}

static void on_line_clear(int num_lines) {

    lines += num_lines;

    if ((level != start_level && lines >= 10) ||
        (level == start_level && (
            lines >= start_level * 10 + 10 ||
            lines >= MAX(100, start_level * 10 - 50)))
    ) {

        level++;
        lines -= 10;

        state.gamemode.gravity_factor = gravity_factor_table[level];
        state.gamemode.soft_drop_factor = state.gamemode.gravity_factor / 2;
        update_colours();

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

static void update_colours(void) {
    piece_colours[T] = nes_colour_palette[nes_level_colours[level][1]];
    piece_colours[J] = nes_colour_palette[nes_level_colours[level][2]];
    piece_colours[Z] = nes_colour_palette[nes_level_colours[level][3]];
    piece_colours[O] = nes_colour_palette[nes_level_colours[level][1]];
    piece_colours[S] = nes_colour_palette[nes_level_colours[level][2]];
    piece_colours[L] = nes_colour_palette[nes_level_colours[level][3]];
    piece_colours[I] = nes_colour_palette[nes_level_colours[level][1]];
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
    .instant_drop_type = Drop_NoInstant,
    .socd_allow_das_overwrite = SOCD_Right,
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

const Menu nes_menu = {
    .menu_items = (MenuItem[2]) {
        NUM_BOX("Start Level:", 0, 19, start_level),
        BUTTON_LOAD_GAMEMODE("Start", nes_mode),
    },
    .menu_item_count = 2
};
