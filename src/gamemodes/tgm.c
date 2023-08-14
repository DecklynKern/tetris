#include <math.h>
#include <string.h>

#include "../../include/tgm.h"

#define HISTORY_LEN 4
#define NUM_TRIES 4

static const Uint32 piece_colours[9] = {
    (Uint32)RGB(  0,   0,   0),
    (Uint32)RGB(255, 255,   0),
    (Uint32)RGB(255,   0, 255),
    (Uint32)RGB(  0, 255,   0),
    (Uint32)RGB(255, 128,   0),
    (Uint32)RGB(  0,   0, 255),
    (Uint32)RGB(  0, 255, 255),
    (Uint32)RGB(255,   0,   0),
    (Uint32)RGB(200, 200, 200)
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

static void tgm1_init() {

    history[0] = history[1] = history[2] = history[3] = Z;

    next_piece = rand() % 4 + 4;
    history[0] = next_piece;

    grade = (char*)tgm1_grade_table[0].name;

}

static void tap_init() {

    history[0] = history[3] = Z;
    history[1] = history[2] = S;

    next_piece = rand() % 4 + 4;
    history[0] = next_piece;

    grade = "";

}

static void tap_master_init() {
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

    int bravo = 4;

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (state.board.minos[y][x]) {
                bravo = 1;
                goto no_bravo;
            }
        }
    }
    no_bravo:

    combo += 2 * num_lines - 2;
    score += ((level + num_lines + 3) / 4 + soft) * num_lines * combo * bravo;

    level += num_lines;

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

static void tap_death_on_line_clear(int num_lines) {

    tgm_on_line_clear(num_lines);
    
    int i;
    for (i = 1; i < TAP_NUM_SPEED_LEVELS && tap_death_speed_table[i].level <= level; i++);
    state.gamemode.are_delay = tap_death_speed_table[i - 1].are_delay;
    state.gamemode.das_delay = tap_death_speed_table[i - 1].das_delay;
    state.gamemode.lock_delay = tap_death_speed_table[i - 1].lock_delay;
    state.gamemode.line_clear_delay = tap_death_speed_table[i - 1].line_clear_delay;

}

static MinoType generate_new_piece() {

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

static void tgm_update() {

    if (state.input_held[Input_Down]) {
        soft++;
    }
}

static void tap_master_update() {

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

static void draw() {

    for (int y = 0; y < BOARD_HEIGHT - INVISIBLE_ROWS; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            draw_mino(x, y + 5, state.board.minos[y + INVISIBLE_ROWS][x], piece_colours);
        }
    }

    if (!state.line_clear_timer && !state.are_timer) {
        for (int i = 0; i < PIECE_MINO_COUNT; i++) {
            draw_mino(
                state.piece.x + get_piece_minos()[i].x,
                state.piece.y + get_piece_minos()[i].y - INVISIBLE_ROWS + 5,
                state.piece.type,
                piece_colours
            );
        }
    }

    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        draw_mino(
            5 + ars_minos[next_piece - 1][Rot_N][i].x,
            ars_minos[next_piece - 1][Rot_N][i].y,
            next_piece,
            piece_colours
        );
    }

    draw_info_text(0, "Grade: %s", grade);
    draw_info_value(1, "Score: %d", score);
    draw_info_value(2, "Level: %d", level);

}

const Gamemode tgm1_mode = {

    .line_clear_delay = 41,
    .are_delay = 30,
    .lock_delay = 30,
    .gravity = 4,
    .gravity_factor = 256,
    .das_delay = 16,
    .arr_delay = 1,
    .soft_drop_factor = 256,

    .can_hold = false,
    .lock_on_down_held = true,
    .num_kicks = 2,
    .piece_rot_minos = &ars_minos,

    .init = tgm1_init,
    .get_kick = get_kick,
    .on_lock = on_lock,
    .on_line_clear = tgm1_on_line_clear,
    .generate_new_piece = generate_new_piece,
    .update = tgm_update,
    .draw = draw

};

const Gamemode tap_master_mode = {

    .line_clear_delay = 40,
    .are_delay = 27,
    .lock_delay = 30,
    .das_delay = 16,
    .arr_delay = 1,
    .soft_drop_factor = 256,
    .gravity = 4,
    .gravity_factor = 256,

    .can_hold = false,
    .lock_on_down_held = true,
    .instant_drop_type = SonicDrop,
    .num_kicks = 2,
    .piece_rot_minos = &ars_minos,

    .init = tap_master_init,
    .get_kick = get_kick,
    .on_lock = on_lock,
    .on_line_clear = tap_master_on_line_clear,
    .generate_new_piece = generate_new_piece,
    .update = tap_master_update,
    .draw = draw

};

const Gamemode tap_tgmplus_mode = {

    .line_clear_delay = 40,
    .are_delay = 27,
    .lock_delay = 30,
    .das_delay = 16,
    .arr_delay = 1,
    .soft_drop_factor = 256,
    .gravity = 4,
    .gravity_factor = 256,

    .can_hold = false,
    .lock_on_down_held = true,
    .instant_drop_type = SonicDrop,
    .num_kicks = 2,
    .piece_rot_minos = &ars_minos,

    .init = tap_init,
    .get_kick = get_kick,
    .on_lock = tap_tgmplus_on_lock,
    .on_line_clear = tgm_on_line_clear,
    .generate_new_piece = generate_new_piece,
    .draw = draw

};

const Gamemode tap_death_mode = {

    .line_clear_delay = 8,
    .are_delay = 18,
    .lock_delay = 30,
    .das_delay = 12,
    .arr_delay = 1,
    .soft_drop_factor = 0,
    .gravity = 5120,
    .gravity_factor = 256,

    .can_hold = false,
    .lock_on_down_held = true,
    .num_kicks = 2,
    .piece_rot_minos = &ars_minos,

    .init = tap_init,
    .get_kick = get_kick,
    .on_lock = on_lock,
    .on_line_clear = tap_death_on_line_clear,
    .generate_new_piece = generate_new_piece,
    .update = tgm_update,
    .draw = draw

};