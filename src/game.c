#include <stdio.h>
#include <string.h>

#include "../include/main.h"
        
static int das_timer;
static int das_direction;
static bool in_lock_delay;
static int lock_delay_timer;
static int gravity_count;
static bool down_held;

static int line_clear_timer;
static int are_timer;

bool board_is_clear(void) {

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (state.board.minos[y][x] != Piece_Empty) {
                return false;
            }
        }
    }
            
    return true;
    
}

bool piece_active() {
    return state.line_clear_timer != -1 || state.are_timer != -1;
}

bool placement_valid(const Point* piece_minos, int piece_x, int piece_y) {
    
    for (int i = 0; i < PIECE_MINO_COUNT; i++) {

        int mino_x = piece_x + piece_minos[i].x;
        int mino_y = piece_y + piece_minos[i].y;

        if (mino_x < 0 || mino_x >= BOARD_WIDTH || mino_y < 0 || mino_y >= BOARD_HEIGHT) {
            return false;
        }

        if (state.board.minos[mino_y][mino_x] != Piece_Empty) {
            return false;
        }
    }

    return true;

}

static bool current_placement_valid(void) {
    return placement_valid(get_piece_minos(), state.piece.x, state.piece.y);
}

static void lock_piece(void) {

    int lines_cleared = 0;

    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        state.board.minos[state.piece.y + get_piece_minos()[i].y][state.piece.x + get_piece_minos()[i].x] = state.piece.type;
    }

    for (int y = 0; y < BOARD_HEIGHT; y++) {

        int full_line = 1;

        for (int x = 0; x < BOARD_WIDTH; x++) {

            if (state.board.minos[y][x] == Piece_Empty) {
                full_line = 0;
                break;
            }
        }

        if (full_line) {

            lines_cleared++;

            for (int above_y = y; above_y > 0; above_y--) {
                memcpy(state.board.minos[above_y], state.board.minos[above_y - 1], ROW_BYTES);
            }

            memset(state.board.minos[0], 0, ROW_BYTES);

        }
    }

    in_lock_delay = false;

    if (state.gamemode.on_lock) {
        state.gamemode.on_lock((bool)lines_cleared);
    }
    
    if (lines_cleared) {
        state.line_clear_timer = state.gamemode.line_clear_delay;
        state.gamemode.on_line_clear(lines_cleared);
    }
    else {
        state.are_timer = state.gamemode.are_delay;
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
    lock_delay_timer = state.gamemode.lock_delay;

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
    state.has_held = false;

    if (!state.gamemode.irs) {
        return;
    }

    if (state.input_held[Input_Rot_CCW]) {
        state.piece.rotation = Rot_W;
    }
    else if (state.input_held[Input_Rot_CW]) {
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

    if (state.line_clear_timer == -1 && state.are_timer == -1) {
        try_move(-1, 0);
    }

    das_timer = state.gamemode.das_delay;
    das_direction = -1;

}

void input_right(void) {

    if (das_direction == -1 && !(state.gamemode.socd_allow_das_overwrite & SOCD_Right)) {
        return;
    }

    if (line_clear_timer == -1 && are_timer == -1) {
        try_move(1, 0);
    }

    das_timer = state.gamemode.das_delay;
    das_direction = 1;
    
}

void input_down(void) {
    down_held = true;
}

void input_instant_drop(void) {

    if (state.gamemode.instant_drop_type == Drop_NoInstant) {
        return;
    }

    while (try_move(0, 1));

    if (state.gamemode.instant_drop_type == Drop_Hard) {
        lock_piece();
    }
}

void input_rotate_ccw(void) {
    if (state.line_clear_timer == -1 && state.are_timer == -1) {
        try_rotate(-1);
    }
}

void input_rotate_cw(void) {
    if (state.line_clear_timer == -1 && state.are_timer == -1) {
        try_rotate(1);
    }
}

void input_hold(void) {

    if (!state.gamemode.can_hold || state.has_held || state.are_timer != -1 || state.line_clear_timer != -1) {
        return;
    }

    if (state.held_piece) {

        MinoType temp = state.held_piece;
        state.held_piece = state.piece.type;
        state.piece.type = temp;

        reset_position();

    }
    else {
        state.held_piece = state.piece.type;
        new_piece();
    }

    state.has_held = true;

}

void release_left(void) {

    if (das_direction == -1) {
        das_direction = 0;
        das_timer = -1;
    }

    if (state.input_held[Input_Right]) {
        input_right();
    }
}

void release_right(void) {

    if (das_direction == 1) {
        das_direction = 0;
        das_timer = -1;
    }

    if (state.input_held[Input_Left]) {
        input_left();
    }
}

void release_down(void) {
    down_held = false;
}

void game_init(void) {

    memset(state.board.minos, Piece_Empty, sizeof(state.board.minos));

    das_timer = -1;
    das_direction = 0;
    in_lock_delay = false;
    gravity_count = 0;
    down_held = false;
    
    state.quit_to_main_menu = false;
    state.held_piece = Piece_Empty;
    state.has_held = false;
    state.line_clear_timer = -1;
    state.are_timer = -1;

    new_piece();

}

const Point* get_piece_minos(void) {
    return (const Point*) &(*state.gamemode.piece_rot_minos)[state.piece.type - 1][state.piece.rotation];
}

void update(void) {

    //TODO: add nes not allowing down + shift

    // technically missing das change restrictions in tgm
    if (das_timer > 0) {
        das_timer--;
    }
    else while (das_timer == 0) {
        
        das_timer = state.gamemode.arr_delay;
    
        if (!try_move(das_direction, 0)) {
            break;
        }
    }

    if (are_timer != -1) {

        are_timer--;

        if (are_timer == -1) {

            new_piece();

            if (!current_placement_valid()) {
                state.quit_to_main_menu = true;
                return;
            }
        }

        return;

    }

    if (line_clear_timer != -1) {

        line_clear_timer--;

        if (line_clear_timer == -1) {
            are_timer = state.gamemode.are_delay;
        }

        return;

    }

    if (down_held) {
        gravity_count += state.gamemode.soft_drop_factor;
    }

    gravity_count += state.gamemode.gravity;

    while (gravity_count > state.gamemode.gravity_factor) {

        gravity_count -= state.gamemode.gravity_factor;

        if (try_move(0, 1)) {
            continue;
        }

        if (!in_lock_delay) {
            in_lock_delay = true;
            lock_delay_timer = state.gamemode.lock_delay;
        }

        break;

    }

    if (placement_valid(get_piece_minos(), state.piece.x, state.piece.y + 1)) {
        in_lock_delay = false;
        lock_delay_timer = state.gamemode.lock_delay;
    }
    else if (in_lock_delay) {

        gravity_count = 0;

        if (!lock_delay_timer || (down_held && state.gamemode.lock_on_down_held)) {
            lock_piece();
        }
        else {
            lock_delay_timer--;
        }
    }

    if (state.gamemode.update) {
        state.gamemode.update();
    }
}
