#include <string.h>
#include <stdlib.h>

#include "../include/main.h"

#define L_MINOS {{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}
#define J_MINOS {{-1, 0}, {0, 0}, {1, 0}, {1, 1}}
#define O_MINOS {{0, 0}, {1, 0}, {0, 1}, {1, 1}}
#define S_MINOS {{0, 0}, {1, 0}, {-1, 1}, {0, 1}}
#define Z_MINOS {{-1, 0}, {0, 0}, {0, 1}, {1, 1}}
#define T_MINOS {{-1, 0}, {0, 0}, {1, 0}, {0, 1}}
#define I_MINOS {{-1, 0}, {0, 0}, {1, 0}, {2, 0}}

void next_piece(Piece* next_piece) {

    next_piece->x = 4;
    next_piece->y = 0;

    int piece_minos_size = PIECE_MINO_COUNT * 2 * sizeof(int);
    
    switch (rand() % 7) {
        case 0:
        {
            int minos[PIECE_MINO_COUNT][2] = L_MINOS;
            memcpy(next_piece, &minos, piece_minos_size);
            next_piece->type = L;
            break;
        }

        case 1:
        {
            int minos[PIECE_MINO_COUNT][2] = J_MINOS;
            memcpy(next_piece, &minos, piece_minos_size);
            next_piece->type = J;
            break;
        }

        case 2:
        {
            int minos[PIECE_MINO_COUNT][2] = O_MINOS;
            memcpy(next_piece, &minos, piece_minos_size);
            next_piece->type = O;
            break;
        }

        case 3:
        {
            int minos[PIECE_MINO_COUNT][2] = S_MINOS;
            memcpy(next_piece, &minos, piece_minos_size);
            next_piece->type = S;
            break;
        }

        case 4:
        {
            int minos[PIECE_MINO_COUNT][2] = Z_MINOS;
            memcpy(next_piece, &minos, piece_minos_size);
            next_piece->type = Z;
            break;
        }

        case 5:
        {
            int minos[PIECE_MINO_COUNT][2] = T_MINOS;
            memcpy(next_piece, &minos, piece_minos_size);
            next_piece->type = T;
            break;
        }

        case 6:
        {
            int minos[PIECE_MINO_COUNT][2] = I_MINOS;
            memcpy(next_piece, &minos, piece_minos_size);
            next_piece->type = I;
            break;
        }
    }
}

int placement_valid(Board* board, int(*piece_minos)[PIECE_MINO_COUNT][2], int piece_x, int piece_y) {
    
    for (int i = 0; i < PIECE_MINO_COUNT; i++) {

        int mino_x = piece_x + (*piece_minos)[i][0];
        int mino_y = piece_y + (*piece_minos)[i][1];

        if (mino_x < 0 || mino_x >= BOARD_WIDTH || mino_y < 0 || mino_y >= BOARD_HEIGHT) {
            return 0;
        }

        if (board->minos[mino_x][mino_y] != None) {
            return 0;
        }
    }

    return 1;

}

void try_move(Board* board, Piece* piece, int dx, int dy) {
    if (placement_valid(board, &piece->minos, piece->x + dx, piece->y + dy)) {
        piece->x += dx;
        piece->y += dy;
    }
}

void lock_piece(Board* board, Piece* piece) {
    for (int i = 0; i < PIECE_MINO_COUNT; i++) {
        board->minos[piece->x + piece->minos[i][0]][piece->y + piece->minos[i][1]] = piece->type;
    }
}