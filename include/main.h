#include <SDL2/SDL.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define SCALE 20

#define PIECE_MINO_COUNT 4

#define SOFT_DROP_DELAY 1
#define LOCK_DELAY 30

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

struct tagPiece {
    int minos[PIECE_MINO_COUNT][2];
    MinoType type;
    int x;
    int y;
};
typedef struct tagPiece Piece;

struct tagBoard {
    MinoType minos[BOARD_WIDTH][BOARD_HEIGHT];
};
typedef struct tagBoard Board;

#define RGB(r, g, b) ((r) << 24 | (g) << 16 | (b) << 8 | 0xFF)
#define GET_R(c)  (c >> 24)
#define GET_G(c)  ((c << 8) >> 24)
#define GET_B(c) ((c << 16) >> 24)
#define GET_A(c) ((c << 24) >> 24)

#define COLOUR_NONE (Uint32)RGB(  0,   0,   0)
#define COLOUR_L    (Uint32)RGB(255, 128,   0)
#define COLOUR_J    (Uint32)RGB(  0,   0, 255)
#define COLOUR_O    (Uint32)RGB(255, 255,   0)
#define COLOUR_S    (Uint32)RGB(255,   0, 255)
#define COLOUR_Z    (Uint32)RGB(  0, 255,   0)
#define COLOUR_T    (Uint32)RGB(  0, 255, 255)
#define COLOUR_I    (Uint32)RGB(255,   0,   0)

void next_piece(Piece* next_piece);
int placement_valid(Board* board, int(*piece_minos)[PIECE_MINO_COUNT][2], int piece_x, int piece_y);
void try_move(Board* board, Piece* piece_mino, int dx, int dy);
void lock_piece(Board* board, Piece* piece_minos);

void draw_mino(SDL_Renderer* renderer, int x, int y, MinoType type);