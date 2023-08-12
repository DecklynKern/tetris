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

#define PIECE_MINO_COUNT 4
#define PIECE_MINOS_BYTES (PIECE_MINO_COUNT * 2 * sizeof(int))

#define FONT_SIZE 16

typedef enum {
    Input_Left = 0,
    Input_Right,
    Input_Down,
    Input_InstantDrop,
    Input_Rot_CW,
    Input_Rot_CCW,
    NUM_HOLDABLE_KEYS
} Inputs;

typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    Empty = 0,
    O,
    S,
    Z,
    L,
    J,
    T,
    I,
    Garbage
} MinoType;

typedef enum {
    Rot_N = 0,
    Rot_E,
    Rot_S,
    Rot_W
} Rotation;

typedef enum {
    NoInstantDrop = 0,
    SonicDrop,
    HardDrop
} InstantDropType;

typedef struct {
    int das_timer;
    bool das;
    int das_direction;
    int lock_delay_timer;
    int gravity_count;
    bool down_held;
} MovementData;

typedef struct {
    Point minos[PIECE_MINO_COUNT];
    MinoType type;
    int x;
    int y;
    Rotation rotation;
} Piece;

typedef struct {
    MinoType minos[BOARD_HEIGHT][BOARD_WIDTH];
} Board;

struct tagGameData;
typedef struct tagGameData GameData;
typedef struct {

    int line_clear_delay;
    int are_delay;
    int lock_delay;
    int das_delay;
    int soft_drop_factor;
    int gravity;
    int gravity_factor;

    const bool can_hold;
    const InstantDropType instant_drop_type;
    const int num_kicks;
    const Point (*piece_rot_minos)[7][4][PIECE_MINO_COUNT];
    
    void (*init)();
    Point (*get_kick)(Rotation new_rotation, int attempt);
    void (*on_lock)(bool cleared_lines);
    void (*on_line_clear)(int num_lines);
    MinoType (*generate_next_piece)();
    void (*update)();
    void (*draw)();

} Gamemode;

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
void input_instant_drop();
void input_rotate_cw();
void input_rotate_ccw();

void release_left();
void release_right();

void game_init();
bool update();

// draw.c
void init_font();
void draw_mino(int cell_x, int cell_y, MinoType type);
void draw_text(int x, int y, const char* text);
void draw_info_value(int row, const char* format, int value);
void draw_info_text(int row, const char* format, char* text);

// mode.c
bool load_gamemode(int argc, char* argv[]);

// main.c
extern SDL_Renderer* renderer;
extern GameData state;

#endif