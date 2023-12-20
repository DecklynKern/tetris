#include <stdio.h>
#include <string.h>

#include "../include/main.h"
#include "../include/input.h"

MinoType board[BOARD_HEIGHT][BOARD_WIDTH];
        
static int das_timer;
static int das_direction;
static int lock_delay_timer;
static int gravity_count;
static bool down_held;

static bool piece_had_down_held_movement;
static int move_rotate_lock_reset_count;

static int line_clear_timer;
static int are_timer;

static bool has_held;
static MinoType held_piece;

static int total_lines_cleared;

bool down_is_held(void) {
    return down_held;
}

MinoType get_held_piece(void) {
    return held_piece;
}

bool board_is_clear(void) {

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x] != Piece_Empty) {
                return false;
            }
        }
    }
            
    return true;
    
}

bool piece_active() {
    return line_clear_timer == -1 && are_timer == -1;
}

bool placement_valid(const Point* piece_minos, int piece_x, int piece_y) {
    
    for (int i = 0; i < PIECE_MINO_COUNT; i++) {

        int mino_x = piece_x + piece_minos[i].x;
        int mino_y = piece_y + piece_minos[i].y;

        if (mino_x < 0 || mino_x >= BOARD_WIDTH || mino_y < 0 || mino_y >= BOARD_HEIGHT) {
            return false;
        }

        if (board[mino_y][mino_x] != Piece_Empty) {
            return false;
        }
    }

    return true;

}

bool lines_cleared_at(int at) {
    return total_lines_cleared >= at;
}

int get_lines_cleared(void) {
    return total_lines_cleared;
}

void board_clear(void) {
    memset(board, Piece_Empty, sizeof(board));
}

static bool current_placement_valid(void) {
    return placement_valid(get_piece_minos(), state.piece.x, state.piece.y);
}

static void try_move_rotate_reset_lock_delay(void) {
    if (++move_rotate_lock_reset_count < state.gamemode.move_rotate_lock_reset_max) {
        lock_delay_timer = state.gamemode.lock_delay;
    }
}

static void lock_piece(void) {

    int lines_cleared = 0;

    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        board[state.piece.y + get_piece_minos()[i].y][state.piece.x + get_piece_minos()[i].x] = state.piece.type;
    }

    for (int y = 0; y < BOARD_HEIGHT; y++) {

        int full_line = 1;

        for (int x = 0; x < BOARD_WIDTH; x++) {

            if (board[y][x] == Piece_Empty) {
                full_line = 0;
                break;
            }
        }

        if (full_line) {

            lines_cleared++;

            for (int above_y = y; above_y > 0; above_y--) {
                memcpy(board[above_y], board[above_y - 1], ROW_BYTES);
            }

            memset(board[0], 0, ROW_BYTES);

        }
    }

    total_lines_cleared += lines_cleared;
    TRY_CALL(state.gamemode.on_lock, (bool) lines_cleared);
    
    if (lines_cleared) {
        line_clear_timer = state.gamemode.line_clear_delay;
        TRY_CALL(state.gamemode.on_line_clear, lines_cleared);
    }
    else {
        are_timer = state.gamemode.are_delay;
    }
}

static bool try_move(int dx, int dy) {

    if (placement_valid(get_piece_minos(), state.piece.x + dx, state.piece.y + dy)) {

        state.piece.x += dx;
        state.piece.y += dy;

        return true;

    }

    return false;

}

static void try_rotate(int rotation_amount) {

    Rotation new_rotation = (state.piece.rotation + rotation_amount) % 4;
    const Point *new_minos = (const Point*)(*state.gamemode.piece_rot_minos)[state.piece.type - 1][new_rotation];

    int attempt = 1;
    Point kick = {0, 0};

    while (!placement_valid(new_minos, state.piece.x + kick.x, state.piece.y + kick.y)) {

        if (attempt > state.gamemode.num_kicks) {
            return;
        }
        
        kick = state.gamemode.get_kick(new_rotation, attempt);
        attempt++;

    }

    state.piece.x += kick.x;
    state.piece.y += kick.y;
    state.piece.rotation = new_rotation;

    try_move_rotate_reset_lock_delay();

}

static void reset_position(void) {

    state.piece.x = 4;
    state.piece.y = INVISIBLE_ROWS;
    state.piece.rotation = Rot_N;

    lock_delay_timer = state.gamemode.lock_delay;

}

static void new_piece(void) {

    reset_position();

    state.piece.type = (*state.gamemode.generate_new_piece)();
    has_held = false;
    piece_had_down_held_movement = false;
    move_rotate_lock_reset_count = 0;
    lock_delay_timer = -1;

    if (!state.gamemode.irs) {
        return;
    }

    if (input_held[Input_Rot_CCW]) {
        state.piece.rotation = Rot_W;
    }
    else if (input_held[Input_Rot_CW]) {
        state.piece.rotation = Rot_E;    
    }
    else {
        state.piece.rotation = Rot_N;
    }

    if (!current_placement_valid()) {
        state.piece.rotation = Rot_N;
    }
}

void input_left(void) {

    if (das_direction == 1 && !(state.gamemode.socd_allow_das_overwrite & SOCD_Left)) {
        return;
    }

    if (piece_active()) {
        if (try_move(-1, 0)) {
            try_move_rotate_reset_lock_delay();
        }
    }

    das_timer = state.gamemode.das_delay;
    das_direction = -1;

}

void input_right(void) {

    if (das_direction == -1 && !(state.gamemode.socd_allow_das_overwrite & SOCD_Right)) {
        return;
    }

    if (piece_active()) {
        if (try_move(1, 0)) {
            try_move_rotate_reset_lock_delay();
        }
    }

    das_timer = state.gamemode.das_delay;
    das_direction = 1;
    
}

void input_down(void) {
    piece_had_down_held_movement = true;
    down_held = true;
}

void input_instant_drop(void) {

    if (!piece_active()) {
        return;
    }

    if (state.gamemode.instant_drop_type == Drop_NoInstant) {
        return;
    }

    while (try_move(0, 1));

    if (state.gamemode.instant_drop_type == Drop_Hard) {
        lock_piece();
    }
}

void input_rotate_ccw(void) {
    if (piece_active()) {
        try_rotate(-1);
    }
}

void input_rotate_cw(void) {
    if (piece_active()) {
        try_rotate(1);
    }
}

void input_hold(void) {

    if (!state.gamemode.can_hold || has_held || !piece_active()) {
        return;
    }

    if (held_piece) {

        MinoType temp = held_piece;
        held_piece = state.piece.type;
        state.piece.type = temp;

        reset_position();

    }
    else {
        held_piece = state.piece.type;
        new_piece();
    }

    has_held = true;

}

void release_left(void) {

    if (das_direction == -1) {
        das_direction = 0;
        das_timer = -1;
    }

    if (input_held[Input_Right]) {
        input_right();
    }
}

void release_right(void) {

    if (das_direction == 1) {
        das_direction = 0;
        das_timer = -1;
    }

    if (input_held[Input_Left]) {
        input_left();
    }
}

void release_down(void) {
    down_held = false;
}

void game_init(void) {

    das_timer = -1;
    das_direction = 0;
    gravity_count = 0;
    down_held = false;
    
    held_piece = Piece_Empty;
    line_clear_timer = -1;
    are_timer = -1;

    has_held = false;

    total_lines_cleared = 0;

    new_piece();

}

const Point* get_piece_minos(void) {
    return (const Point*) &(*state.gamemode.piece_rot_minos)[state.piece.type - 1][state.piece.rotation];
}

static bool apply_gravity(int amount) {

    bool applied_gravity = false;

    gravity_count += amount;

    while (gravity_count > state.gamemode.gravity_factor) {

        gravity_count -= state.gamemode.gravity_factor;

        if (try_move(0, 1)) {
            lock_delay_timer = -1;
            move_rotate_lock_reset_count = 0;
            applied_gravity = true;
            continue;
        }

        if (lock_delay_timer == -1) {
            lock_delay_timer = state.gamemode.lock_delay;
        }

        break;

    }

    return applied_gravity;

}

void update(void) {

    //TODO: add nes not allowing down + shift

    // technically missing das change restrictions in tgm
    if (das_timer > 0) {
        das_timer--;
    }
    
    while (das_timer == 0) {
        
        das_timer = state.gamemode.arr_delay;
    
        if (!piece_active() || !try_move(das_direction, 0)) {
            break;
        }
    }

    if (line_clear_timer != -1) {

        if (--line_clear_timer == -1) {
            are_timer = state.gamemode.are_delay;
        }
        else {
            return;
        }
    }

    if (are_timer != -1) {

        if (--are_timer == -1) {

            new_piece();

            if (!current_placement_valid()) {
                set_exit_game();
                return;
            }
        }
        else {
            return;
        }
    }

    apply_gravity(state.gamemode.gravity);

    if (down_held && apply_gravity(state.gamemode.soft_drop_factor)) {
        piece_had_down_held_movement = true;
    }

    if (placement_valid(get_piece_minos(), state.piece.x, state.piece.y + 1)) {
        lock_delay_timer = -1;
    }
    else if (lock_delay_timer != -1) {

        gravity_count = 0;

        if (!lock_delay_timer || (down_held && state.gamemode.lock_on_down_held && piece_had_down_held_movement)) {
            lock_piece();
        }
        else {
            lock_delay_timer--;
        }
    }

    if (state.gamemode.max_lines && total_lines_cleared >= state.gamemode.max_lines) {
        set_exit_game();
    }

    TRY_CALL(state.gamemode.update);

}
