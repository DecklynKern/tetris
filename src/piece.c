#include <string.h>
#include <stdlib.h>

#include "../include/main.h"

int placement_valid(Board* board, Point piece_minos[PIECE_MINO_COUNT], int piece_x, int piece_y) {
    
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

int lock_piece(Board* board, Piece* piece) {

    int cleared_line = 0;

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

            cleared_line = 1;

            for (int above_y = y; above_y > 0; above_y--) {
                memcpy(board->minos[above_y], board->minos[above_y - 1], ROW_BYTES);
            }

            memset(board->minos[0], 0, ROW_BYTES);

        }
    }

    return cleared_line;

}

int try_move(Board* board, Piece* piece, int dx, int dy) {

    if (placement_valid(board, piece->minos, piece->x + dx, piece->y + dy)) {

        piece->x += dx;
        piece->y += dy;

        return 1;

    }

    return 0;

}

void try_rotate_piece(GameData* state, int rotation_amount) {

    Rotation new_rotation = (state->piece.rotation + rotation_amount) % 4;
    Point new_minos[PIECE_MINO_COUNT];

    memcpy(new_minos, (*state->gamemode.piece_rot_minos)[state->piece.type][new_rotation], PIECE_MINOS_BYTES);

    int attempt = 1;
    Point kick = {0, 0};

    while (!placement_valid(&state->board, new_minos, state->piece.x + kick.x, state->piece.y + kick.y)) {

        if (attempt == state->gamemode.num_kicks) {
            return;
        }
        
        kick = state->gamemode.get_kick(state, new_rotation, attempt);
        attempt += 1;

    }

    state->piece.x += kick.x;
    state->piece.y += kick.y;

    memcpy(state->piece.minos, new_minos, PIECE_MINOS_BYTES);
    state->piece.rotation = new_rotation;

}