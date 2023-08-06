#include <string.h>
#include <stdlib.h>

#include "../include/main.h"

const int piece_rot_minos[8][4][PIECE_MINO_COUNT][2] = {
    {},
    {   // L
        {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{-1, -1}, { 0, -1}, { 0,  0}, { 0,  1}}
    },
    {   // J
        {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}},
        {{-1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}}
    },
    {   // O
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}
    },
    {   // S
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}},
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    },
    {   // Z
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}}
    },
    {   // T
        {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    },
    {   // I
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}}
    }
};

void next_piece(Piece* piece, Rotation irs_rotation) {

    piece->x = 4;
    piece->y = INVISIBLE_ROWS;
    piece->rotation = irs_rotation;

    piece->type = 1 + rand() % 7;
    memcpy(piece->minos, piece_rot_minos[piece->type][irs_rotation], PIECE_MINOS_BYTES);

}

int placement_valid(Board* board, int(*piece_minos)[PIECE_MINO_COUNT][2], int piece_x, int piece_y) {
    
    for (int i = 0; i < PIECE_MINO_COUNT; i++) {

        int mino_x = piece_x + (*piece_minos)[i][0];
        int mino_y = piece_y + (*piece_minos)[i][1];

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
        board->minos[piece->y + piece->minos[i][1]][piece->x + piece->minos[i][0]] = piece->type;
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

    if (placement_valid(board, &piece->minos, piece->x + dx, piece->y + dy)) {

        piece->x += dx;
        piece->y += dy;

        return 1;

    }

    return 0;

}

void try_rotate_piece(Board* board, Piece* piece, int rotation_amount) {

    Rotation new_rotation = (piece->rotation + rotation_amount) % 4;
    int new_minos[PIECE_MINO_COUNT][2];

    memcpy(&new_minos, piece_rot_minos[piece->type][new_rotation], PIECE_MINOS_BYTES);

    if (placement_valid(board, &new_minos, piece->x, piece->y)) {
        memcpy(&piece->minos, new_minos,PIECE_MINOS_BYTES);
        piece->rotation = new_rotation;
        return;
    }

    if (piece->type == I) {
        return;
    }

    if (piece->type == L || piece->type == J || piece->type == T) {

        for (int dy = -1; dy < 2; dy++) {
            for (int dx = -1; dx < 2; dx++) {

                if (board->minos[piece->y + dy][piece->x + dx]) {

                    if (dx == 0) {
                        return;
                    }

                    goto no_centre_column;

                }
            }
        }
    }
    no_centre_column:

    if (placement_valid(board, &new_minos, piece->x + 1, piece->y)) {
        memcpy(&piece->minos, new_minos,PIECE_MINOS_BYTES);
        piece->rotation = new_rotation;
        piece->x++;
        return;
    }

    if (placement_valid(board, &new_minos, piece->x - 1, piece->y)) {
        memcpy(&piece->minos, new_minos,PIECE_MINOS_BYTES);
        piece->rotation = new_rotation;
        piece->x--;
        return;
    }
}