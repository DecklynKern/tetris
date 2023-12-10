#include <math.h>
#include <string.h>

#include "../../include/tgm.h"
#include "../../include/menu.h"

#define HISTORY_LEN 4
#define NUM_TRIES 4

static const Uint32 piece_colours[9] = {
    RGB(  0,   0,   0),
    RGB(255, 255,   0),
    RGB(255,   0, 255),
    RGB(  0, 255,   0),
    RGB(255, 128,   0),
    RGB(  0,   0, 255),
    RGB(  0, 255, 255),
    RGB(255,   0,   0),
    RGB(200, 200, 200)
};

static MinoType next_piece;
static MinoType history[HISTORY_LEN];

static int level = 0;
static int score = 0;
static char* grade;

static int combo = 1;
static int soft = 0;

// tap master
static int internal_grade = 0;
static int decay_timer = 125;
static int internal_score = 0;

// tap tgm+
static int tgmplus_garbage_counter = 0;
static int tgmplus_garbage_row = 0;

static void tgm1_init(void) {

    history[0] = history[1] = history[2] = history[3] = Z;

    next_piece = rand() % 4 + 4;
    history[0] = next_piece;

    grade = (char*)tgm1_grade_table[0].name;

}

static void tap_init(void) {

    history[0] = history[3] = Z;
    history[1] = history[2] = S;

    next_piece = rand() % 4 + 4;
    history[0] = next_piece;

    grade = "";

}

static void tap_master_init(void) {
    tap_init();
    grade = "9";
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

    return (Point) {attempt == 1 ? 1 : -1, 0};

}

static void on_lock(bool cleared_lines) {
    
    if (level % 100 != 99 && level != 998) {
        level++;
    }

    combo = 1;
    soft = 0;

}

static void tap_tgmplus_on_lock(bool cleared_lines) {

    on_lock(cleared_lines);

    if (!cleared_lines) {

        tgmplus_garbage_counter++;

        if (tgmplus_garbage_counter >= 13 - level / 100) {

            for (int y = 1; y < BOARD_HEIGHT; y++) {
                memcpy(state.board.minos[y - 1], state.board.minos[y], ROW_BYTES);
            }

            for (int x = 0; x < BOARD_WIDTH; x++) {
                state.board.minos[BOARD_HEIGHT - 1][x] = tgmplus_garbage_rows[tgmplus_garbage_row][x] ? Garbage : Empty;
            }
        
            tgmplus_garbage_counter = 0;
            tgmplus_garbage_row = (tgmplus_garbage_row + 1) % TGMPLUS_NUM_GARBAGE_ROWS;

        }
    }
}

static void tgm_on_line_clear(int num_lines) {

    int bravo = 1;
    
    if (board_is_clear()) {
        bravo = 4;
    }

    combo += 2 * num_lines - 2;
    score += ((level + num_lines + 3) / 4 + soft) * num_lines * combo * bravo;

    level += num_lines;

    if (level > 100) {
        state.gamemode.show_ghost = false;
    }
}

static void tgm1_on_line_clear(int num_lines) {

    tgm_on_line_clear(num_lines);

    int i;
    for (i = 1; i < TGM1_NUM_GRAVITY_LEVELS && tgm1_gravity_table[i].level <= level; i++);
    state.gamemode.gravity = tgm1_gravity_table[i - 1].gravity;

    for (i = 1; i < NUM_GRADES && tgm1_grade_table[i].score <= score; i++);
    grade = (char*)tgm1_grade_table[i - 1].name;
}

static void tap_master_on_line_clear(int num_lines) {

    tgm_on_line_clear(num_lines);

    int i;
    for (i = 1; i < TGM1_NUM_GRAVITY_LEVELS && tgm1_gravity_table[i].level <= level; i++);
    state.gamemode.gravity = tgm1_gravity_table[i - 1].gravity;
    
    for (i = 1; i < TAP_NUM_SPEED_LEVELS && tap_master_speed_table[i].level <= level; i++);
    state.gamemode.are_delay = tap_master_speed_table[i - 1].are_delay;
    state.gamemode.das_delay = tap_master_speed_table[i - 1].das_delay;
    state.gamemode.lock_delay = tap_master_speed_table[i - 1].lock_delay;
    state.gamemode.line_clear_delay = tap_master_speed_table[i - 1].line_clear_delay;

    // not sure if this uses the same combo system as tgm1 or just an incrementing one
    int base_score = grs_grade_table[internal_grade].line_clear_values[num_lines - 1];
    double combo_multiplier = grs_combo_table[combo - 1][num_lines - 1];
    int level_multiplier = (level / 250) + 1;

    internal_score += (int)ceil(((double) base_score) * combo_multiplier) * level_multiplier;

    if (internal_score > 100) {

        internal_score = 0;
        internal_grade++;

        if (internal_grade == NUM_GRADES) {
            internal_grade = NUM_GRADES - 1;
        }

        grade = (char*)grs_grade_table[internal_grade].displayed_grade;

    }
}

static void tap_tgmplus_on_line_clear(int num_lines) {

    tgm_on_line_clear(num_lines);

    int i;
    for (i = 1; i < TGM1_NUM_GRAVITY_LEVELS && tgm1_gravity_table[i].level <= level; i++);
    state.gamemode.gravity = tgm1_gravity_table[i - 1].gravity;

}

static void tap_death_on_line_clear(int num_lines) {

    tgm_on_line_clear(num_lines);
    
    int i;
    for (i = 1; i < TAP_NUM_SPEED_LEVELS && tap_death_speed_table[i].level <= level; i++);
    state.gamemode.are_delay = tap_death_speed_table[i - 1].are_delay;
    state.gamemode.das_delay = tap_death_speed_table[i - 1].das_delay;
    state.gamemode.lock_delay = tap_death_speed_table[i - 1].lock_delay;
    state.gamemode.line_clear_delay = tap_death_speed_table[i - 1].line_clear_delay;

}

static MinoType generate_new_piece(void) {

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

    MinoType new_piece_type = next_piece;
    next_piece = try_piece;

    return new_piece_type;

}

static void tgm_update(void) {

    if (state.input_held[Input_Down]) {
        soft++;
    }
}

static void tap_master_update(void) {

    tgm_update();

    decay_timer--;

    if (!decay_timer) {
        
        internal_score--;

        if (internal_score < 0) {
            internal_score = 0;
        }

        decay_timer = grs_grade_table[internal_grade].decay_rate;

    }
}

static void draw(void) {

    draw_board();
    draw_single_next(next_piece);

    draw_info_text(0, "Grade: %s", grade);
    draw_info_value(1, "Score: %d", score);
    draw_info_value(2, "Level: %d", level);
    draw_info_timer(3);
    
}

#define TGM_SETTINGS \
    .gravity_factor = 256,\
    .arr_delay = 1,\
    .soft_drop_factor = 256,\
    \
    .show_ghost = true,\
    .can_hold = false,\
    .lock_on_down_held = true,\
    .irs = true,\
    .socd_allow_das_overwrite = SOCD_Left,\
    .num_kicks = 2,\
    .piece_rot_minos = &ars_minos,\
    .piece_colours = &piece_colours,\
    \
    .draw = draw

#define TAP_SETTINGS \
    TGM_SETTINGS, \
    \
    .fps = 61.680, \
    \
    .instant_drop_type = Drop_Sonic,\
    \
    .get_kick = get_kick,\
    .generate_new_piece = generate_new_piece

const Gamemode tgm1_mode = {

    TGM_SETTINGS,
    
    .fps = 59.84,

    .line_clear_delay = 41,
    .are_delay = 30,
    .lock_delay = 30,
    .gravity = 4,
    .das_delay = 16,

    .instant_drop_type = Drop_NoInstant,

    .init = tgm1_init,
    .get_kick = get_kick,
    .on_lock = on_lock,
    .on_line_clear = tgm1_on_line_clear,
    .generate_new_piece = generate_new_piece,
    .update = tgm_update

};

const Gamemode tap_master_mode = {

    TAP_SETTINGS,

    .line_clear_delay = 40,
    .are_delay = 27,
    .lock_delay = 30,
    .das_delay = 16,
    .gravity = 4,

    .init = tap_master_init,
    .on_lock = on_lock,
    .on_line_clear = tap_master_on_line_clear,
    .update = tap_master_update

};

const Gamemode tap_tgmplus_mode = {

    TAP_SETTINGS,

    .line_clear_delay = 40,
    .are_delay = 27,
    .lock_delay = 30,
    .das_delay = 16,
    .gravity = 4,

    .init = tap_init,
    .on_lock = tap_tgmplus_on_lock,
    .on_line_clear = tap_tgmplus_on_line_clear,
    .update = tgm_update

};

const Gamemode tap_death_mode = {

    TAP_SETTINGS,

    .line_clear_delay = 8,
    .are_delay = 18,
    .lock_delay = 30,
    .das_delay = 12,
    .gravity = 5120,

    .init = tap_init,
    .on_lock = on_lock,
    .on_line_clear = tap_death_on_line_clear,
    .update = tgm_update

};

const Menu tgm_menu = {
    .menu_items = (MenuItem[4]) {
        BUTTON_LOAD_GAMEMODE("TGM1", tgm1_mode),
        BUTTON_LOAD_GAMEMODE("TAP Master", tap_master_mode),
        BUTTON_LOAD_GAMEMODE("TAP TGM+", tap_tgmplus_mode),
        BUTTON_LOAD_GAMEMODE("TAP Death", tap_death_mode)
    },
    .menu_item_count = 4
};