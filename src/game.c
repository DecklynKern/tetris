#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>

#include "../include/main.h"

static int placement_valid(Board* board, Point piece_minos[PIECE_MINO_COUNT], int piece_x, int piece_y) {
    
    for (int i = 0; i < PIECE_MINO_COUNT; i++) {

        int mino_x = piece_x + piece_minos[i].x;
        int mino_y = piece_y + piece_minos[i].y;

        if (mino_x < 0 || mino_x >= BOARD_WIDTH || mino_y < 0 || mino_y >= BOARD_HEIGHT) {
            return 0;
        }

        if (board->minos[mino_y][mino_x] != None) {
            return 0;
        }
    }

    return 1;

}

static int lock_piece(Board* board, Piece* piece) {

    int lines_cleared = 0;

    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        board->minos[piece->y + piece->minos[i].y][piece->x + piece->minos[i].x] = piece->type;
    }

    for (int y = 0; y < BOARD_HEIGHT; y++) {

        int full_line = 1;

        for (int x = 0; x < BOARD_WIDTH; x++) {

            if (board->minos[y][x] == None) {
                full_line = 0;
                break;
            }
        }

        if (full_line) {

            lines_cleared++;

            for (int above_y = y; above_y > 0; above_y--) {
                memcpy(board->minos[above_y], board->minos[above_y - 1], ROW_BYTES);
            }

            memset(board->minos[0], 0, ROW_BYTES);

        }
    }

    return lines_cleared;

}

static int try_move(Board* board, Piece* piece, int dx, int dy) {

    if (placement_valid(board, piece->minos, piece->x + dx, piece->y + dy)) {

        piece->x += dx;
        piece->y += dy;

        return 1;

    }

    return 0;

}

static void try_rotate_piece(int rotation_amount) {

    Rotation new_rotation = (state.piece.rotation + rotation_amount) % 4;
    Point new_minos[PIECE_MINO_COUNT];

    memcpy(new_minos, (*state.gamemode.piece_rot_minos)[state.piece.type][new_rotation], PIECE_MINOS_BYTES);

    int attempt = 1;
    Point kick = {0, 0};

    while (!placement_valid(&state.board, new_minos, state.piece.x + kick.x, state.piece.y + kick.y)) {

        if (attempt > state.gamemode.num_kicks) {
            return;
        }
        
        kick = state.gamemode.get_kick(new_rotation, attempt);
        attempt += 1;

    }

    state.piece.x += kick.x;
    state.piece.y += kick.y;

    memcpy(state.piece.minos, new_minos, PIECE_MINOS_BYTES);
    state.piece.rotation = new_rotation;

}

static void new_piece() {

    state.piece.x = 4;
    state.piece.y = INVISIBLE_ROWS;
    state.piece.rotation = Rot_N;

    state.piece.type = (*state.gamemode.generate_next_piece)();
    memcpy(state.piece.minos, (*state.gamemode.piece_rot_minos)[state.piece.type][state.piece.rotation], PIECE_MINOS_BYTES);

    state.movement.lock_delay_timer = state.gamemode.lock_delay;

}

void input_left() {

    if (!state.line_clear_timer && !state.are_timer) {
        try_move(&state.board, &state.piece, -1, 0);
    }

    state.movement.das_timer = state.gamemode.das_delay;
    state.movement.das = false;
    state.movement.das_direction = -1;

}

void input_right() {

    if (!state.line_clear_timer && !state.are_timer) {
        try_move(&state.board, &state.piece, 1, 0);
    }

    state.movement.das_timer = state.gamemode.das_delay;
    state.movement.das = false;
    state.movement.das_direction = 1;

}

void input_down() {
    state.movement.down_held = true;
}

void input_rotate_cw() {
    if (!state.line_clear_timer && !state.are_timer) {
        try_rotate_piece(1);
    }
}

void input_rotate_ccw() {
    if (!state.line_clear_timer && !state.are_timer) {
        try_rotate_piece(-1);
    }
}

void release_left() {
    if (state.movement.das_direction == -1 && (state.movement.das_timer || state.movement.das)) {
        state.movement.das_timer = 0;
        state.movement.das = 0;
        state.movement.das_direction = 0;
    }
}

void release_right() {
    if (state.movement.das_direction == 1 && (state.movement.das_timer || state.movement.das)) {
        state.movement.das_timer = 0;
        state.movement.das = 0;
        state.movement.das_direction = 0;
    }
}

void release_down() {
    state.movement.down_held = false;
}

void game_init() {
    new_piece();
}

bool update() {

    // technically missing das change restrictions in tgm
    if (state.movement.das_timer) {

        state.movement.das_timer--;

        if (!state.movement.das_timer) {
            state.movement.das = 1;
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
        
            if (!placement_valid(&state.board, state.piece.minos, state.piece.x, state.piece.y)) {
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

        if (!try_move(&state.board, &state.piece, 0, 1)) {
            break;
        }
        
        state.movement.lock_delay_timer = state.gamemode.lock_delay;

    }

    if (state.movement.das) {
        try_move(&state.board, &state.piece, state.movement.das_direction, 0);
    }

    if (placement_valid(&state.board, state.piece.minos, state.piece.x, state.piece.y + 1)) {
        state.movement.lock_delay_timer = state.gamemode.lock_delay;

    } else {

        state.movement.gravity_count = 0;

        if (!state.movement.lock_delay_timer || state.input_held[Input_Down]) {

            int lines_cleared = lock_piece(&state.board, &state.piece);

            state.gamemode.on_lock();
            
            if (lines_cleared) {
                state.line_clear_timer = state.gamemode.line_clear_delay;
                state.gamemode.on_line_clear(lines_cleared);

            } else {
                state.are_timer = state.gamemode.are_delay;
            }

        } else {
            state.movement.lock_delay_timer--;
        }
    }

    if (state.gamemode.update) {
        state.gamemode.update();
    }

    return false;

}