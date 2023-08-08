#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <stdbool.h>

#ifndef MAIN
#define MAIN

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 40
#define INVISIBLE_ROWS 20
#define SCALE 30
#define ROW_BYTES BOARD_WIDTH * sizeof(MinoType)
#define MAX_KICKS 5

#define GRAVITY_FACTOR 256

#define PIECE_MINO_COUNT 4
#define PIECE_MINOS_BYTES (PIECE_MINO_COUNT * 2 * sizeof(int))

#define FONT_SIZE 16

enum tagInputs {
    Input_Left = 0,
    Input_Right,
    Input_Down,
    Input_Rot_CW,
    Input_Rot_CCW,
    NUM_HOLDABLE_KEYS
};
typedef enum tagInputs Inputs;

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
    Rot_E,
    Rot_S,
    Rot_W
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

    int screen_width;
    int screen_height;

    int line_clear_delay;
    int are_delay;
    int lock_delay;
    int gravity;
    int das_delay;
    int soft_drop_factor;

    const bool can_hold;
    const int num_kicks;
    const Point (*piece_rot_minos)[8][4][PIECE_MINO_COUNT];
    
    void (*init)();
    Point (*get_kick)(Rotation new_rotation, int attempt);
    void (*on_lock)();
    void (*on_line_clear)(int num_lines);
    MinoType (*generate_next_piece)();
    void (*draw)(SDL_Renderer* renderer);

};
typedef struct tagGamemode Gamemode;

struct tagGameData {
    bool input_held[NUM_HOLDABLE_KEYS];
    Piece piece;
    MovementData movement;
    Board board;
    Gamemode gamemode;
    int line_clear_timer;
    int are_timer;
};

// game.c
void input_left();
void input_right();
void input_rotate_cw();
void input_rotate_ccw();

void release_left();
void release_right();

void game_init();
bool update();

// draw.c
void init_font();
void draw_mino(SDL_Renderer* renderer, int cell_x, int cell_y, MinoType type);
void draw_text(SDL_Renderer* renderer, int x, int y, const char* text);

// mode.c
bool load_gamemode(int argc, char* argv[]);

// main.c
extern GameData state;

#endif