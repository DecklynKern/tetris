#include <SDL2/SDL.h>

#ifndef MAIN
#define MAIN

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 30
#define INVISIBLE_ROWS 10
#define SCALE 30
#define ROW_BYTES BOARD_WIDTH * sizeof(MinoType)

#define PIECE_MINO_COUNT 4
#define PIECE_MINOS_BYTES (PIECE_MINO_COUNT * 2 * sizeof(int))

#define LINE_CLEAR_DELAY 41
#define ARE_DELAY 30
#define LOCK_DELAY 30
#define GRAVITY_FACTOR 256
#define DAS_DELAY 16
#define SOFT_DROP_FACTOR 256
#define GRAVITY 4

enum tagMinoType {
    None = 0,
    L,
    J,
    O,
    S,
    Z,
    T,
    I
};
typedef enum tagMinoType MinoType;

enum tagRotation {
    Rot_N = 0,
    Rot_W,
    Rot_S,
    Rot_E
};
typedef enum tagRotation Rotation;

struct tagPiece {
    int minos[PIECE_MINO_COUNT][2];
    MinoType type;
    int x;
    int y;
    Rotation rotation;
};
typedef struct tagPiece Piece;

struct tagBoard {
    MinoType minos[BOARD_HEIGHT][BOARD_WIDTH];
};
typedef struct tagBoard Board;

extern const int piece_rot_minos[8][4][PIECE_MINO_COUNT][2];

#define RGB(r, g, b) ((r) << 24 | (g) << 16 | (b) << 8 | 0xFF)
#define GET_R(c)  (c >> 24)
#define GET_G(c)  ((c << 8) >> 24)
#define GET_B(c) ((c << 16) >> 24)
#define GET_A(c) ((c << 24) >> 24)

extern const Uint32 piece_colours[8];

void next_piece(Piece* next_piece, Rotation irs_rotation);
int placement_valid(Board* board, int(*piece_minos)[PIECE_MINO_COUNT][2], int piece_x, int piece_y);
int lock_piece(Board* board, Piece* piece);
int try_move(Board* board, Piece* piece, int dx, int dy);
void try_rotate_piece(Board* board, Piece* piece, int rotation_amount);

void draw_mino(SDL_Renderer* renderer, int x, int y, MinoType type);

#endif