#include <stdio.h>
#include <string.h>

#include "../include/main.h"

static bool placement_valid(const Point* piece_minos, int piece_x, int piece_y) {
    
    for (int i = 0; i < PIECE_MINO_COUNT; i++) {

        int mino_x = piece_x + piece_minos[i].x;
        int mino_y = piece_y + piece_minos[i].y;

        if (mino_x < 0 || mino_x >= BOARD_WIDTH || mino_y < 0 || mino_y >= BOARD_HEIGHT) {
            return false;
        }

        if (state.board.minos[mino_y][mino_x] != Empty) {
            return false;
        }
    }

    return true;

}

static bool current_placement_valid() {
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

            if (state.board.minos[y][x] == Empty) {
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

    if (state.gamemode.on_lock) {
        state.gamemode.on_lock((bool)lines_cleared);
    }
    
    if (lines_cleared) {

        if (state.gamemode.line_clear_delay) {
            state.line_clear_timer = state.gamemode.line_clear_delay;
        
        } else {
            state.are_timer = state.gamemode.are_delay;
        }
        
        state.gamemode.on_line_clear(lines_cleared);

    } else {
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

}

static void reset_position(void) {

    state.piece.x = 4;
    state.piece.y = INVISIBLE_ROWS;
    state.piece.rotation = Rot_N;

    state.movement.lock_delay_timer = state.gamemode.lock_delay;

}


static void new_piece(void) {

    reset_position();

    state.piece.type = (*state.gamemode.generate_new_piece)();
    state.has_held = false;

    if (state.gamemode.irs) {

        if (state.input_held[Input_Rot_CCW]) {
            state.piece.rotation = Rot_W;

        } else if (state.input_held[Input_Rot_CW]) {
            state.piece.rotation = Rot_E;
            
        } else {
            state.piece.rotation = Rot_N;
        }

        if (!current_placement_valid()) {
            state.piece.rotation = Rot_N;
        }
    }
}

void input_left(void) {

    if (!state.line_clear_timer && !state.are_timer) {
        try_move(-1, 0);
    }

    state.movement.das_timer = state.gamemode.das_delay;
    state.movement.das_direction = -1;

}

void input_right(void) {

    if (!state.line_clear_timer && !state.are_timer) {
        try_move(1, 0);
    }

    state.movement.das_timer = state.gamemode.das_delay;
    state.movement.das_direction = 1;

}

void input_down(void) {
    state.movement.down_held = true;
}

void input_instant_drop(void) {

    if (state.gamemode.instant_drop_type == NoInstantDrop) {
        return;
    }

    while (placement_valid(get_piece_minos(), state.piece.x, state.piece.y + 1)) {
        state.piece.y++;
    }

    if (state.gamemode.instant_drop_type == HardDrop) {
        lock_piece();
    }
}

void input_rotate_cw(void) {
    if (!state.line_clear_timer && !state.are_timer) {
        try_rotate(1);
    }
}

void input_rotate_ccw(void) {
    if (!state.line_clear_timer && !state.are_timer) {
        try_rotate(-1);
    }
}

void input_hold(void) {

    if (!state.gamemode.can_hold || state.has_held) {
        return;
    }

    if (state.held_piece) {

        MinoType temp = state.held_piece;
        state.held_piece = state.piece.type;
        state.piece.type = temp;

        reset_position();

    } else {
        state.held_piece = state.piece.type;
        new_piece();
    }

    state.has_held = true;

}

void release_left(void) {
    state.movement.das_timer = 0;
}

void release_right(void) {
    state.movement.das_timer = 0;
}

void release_down(void) {
    state.movement.down_held = false;
}

void game_init(void) {
    new_piece();
}

const Point* get_piece_minos(void) {
    return (const Point*)&(*state.gamemode.piece_rot_minos)[state.piece.type - 1][state.piece.rotation];
}

bool update(void) {

    // technically missing das change restrictions in tgm
    if (state.movement.das_timer) {

        state.movement.das_timer--;

        if (!state.movement.das_timer) {
            state.movement.das_timer = state.gamemode.arr_delay;
            try_move(state.movement.das_direction, 0);
        }
    }

    if (state.line_clear_timer) {

        state.line_clear_timer--;

        if (!state.line_clear_timer) {
            state.are_timer = state.gamemode.are_delay;
        }

        return false;

    }

    if (state.are_timer) {

        state.are_timer--;

        if (!state.are_timer) {

            new_piece();

            if (!current_placement_valid()) {
                return true;
            }
        }

        return false;

    }

    if (state.input_held[Input_Down]) {
        state.movement.gravity_count += state.gamemode.soft_drop_factor;
    }

    state.movement.gravity_count += state.gamemode.gravity;

    while (state.movement.gravity_count > state.gamemode.gravity_factor) {

        state.movement.gravity_count -= state.gamemode.gravity_factor;

        if (!try_move(0, 1)) {
            break;
        }
        
        state.movement.lock_delay_timer = state.gamemode.lock_delay;

    }

    if (placement_valid(get_piece_minos(), state.piece.x, state.piece.y + 1)) {
        state.movement.lock_delay_timer = state.gamemode.lock_delay;

    } else {

        state.movement.gravity_count = 0;

        if (!state.movement.lock_delay_timer || (state.input_held[Input_Down] && state.gamemode.lock_on_down_held)) {
            lock_piece();

        } else {
            state.movement.lock_delay_timer--;
        }
    }

    if (state.gamemode.update) {
        state.gamemode.update();
    }

    return false;

}