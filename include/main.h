#include <SDL2/SDL.h>
#include <stdbool.h>

#ifndef MAIN
#define MAIN

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 40
#define INVISIBLE_ROWS 20
#define SCALE 30
#define ROW_BYTES BOARD_WIDTH * sizeof(MinoType)
#define MAX_KICKS 5

#define PIECE_MINO_COUNT 4
#define PIECE_MINOS_BYTES (PIECE_MINO_COUNT * 2 * sizeof(int))

enum tagHoldableKeys {
    Input_Left = 0,
    Input_Right,
    Input_Down,
    Input_Rot_CW,
    Input_Rot_CCW,
    _Num_Holdable_Keys
};
typedef enum tagHoldableKeys HoldableKeys;

struct tagPoint {
    int x;
    int y;
};
typedef struct tagPoint Point;

enum tagMinoType {
    None = 0,
    O,
    S,
    Z,
    L,
    J,
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

struct tagMovementData {
    int das_timer;
    bool das;
    int das_direction;
    int lock_delay_timer;
    int gravity_count;
    bool down_held;
};
typedef struct tagMovementData MovementData;

struct tagPiece {
    Point minos[PIECE_MINO_COUNT];
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

struct tagGameData;
typedef struct tagGameData GameData;
struct tagGamemode {
    int line_clear_delay;
    int are_delay;
    int lock_delay;
    int gravity_factor;
    int das_delay;
    int soft_drop_factor;
    int gravity;
    const bool can_hold;
    const int num_kicks;
    const Point (*piece_rot_minos)[8][4][PIECE_MINO_COUNT];
    void (*new_piece)(GameData* state);
    Point (*get_kick)(GameData* state, Rotation new_rotation, int attempt);
};
typedef struct tagGamemode Gamemode;

struct tagGameData {
    bool held_inputs[_Num_Holdable_Keys];
    Piece piece;
    MovementData movement;
    Board board;
    Gamemode gamemode;
    int line_clear_timer;
    int are_timer;
};

// game.c
void input_left(GameData* state);
void input_right(GameData* state);
void input_rotate_cw(GameData* state);
void input_rotate_ccw(GameData* state);

void release_left(GameData* state);
void release_right(GameData* state);

bool update(GameData* state);

// piece.c
int placement_valid(Board* board, Point piece_minos[PIECE_MINO_COUNT], int piece_x, int piece_y);
int lock_piece(Board* board, Piece* piece);
int try_move(Board* board, Piece* piece, int dx, int dy);
void try_rotate_piece(GameData* state, int rotation_amount);

// draw.c
extern const Uint32 piece_colours[8];
void draw_mino(SDL_Renderer* renderer, int x, int y, MinoType type);

// tgm.c
extern const Gamemode tgm1_mode;

#endif