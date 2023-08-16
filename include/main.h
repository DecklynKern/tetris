#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>

#ifndef MAIN
#define MAIN

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 40
#define INVISIBLE_ROWS 20
#define SCALE 30
#define ROW_BYTES BOARD_WIDTH * sizeof(MinoType)
#define MAX_KICKS 5

#define TOP_SPACE_HEIGHT 150
#define RIGHT_SPACE_WIDTH 150

#define PIECE_MINO_COUNT 4

#define RGB(r, g, b) ((r) << 24 | (g) << 16 | (b) << 8 | 0xFF)

typedef enum {
    Input_Left = 0,
    Input_Right,
    Input_Down,
    Input_InstantDrop,
    Input_Rot_CW,
    Input_Rot_CCW,
    Input_Hold,
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
    int das_direction;
    int lock_delay_timer;
    int gravity_count;
    bool down_held;
} MovementData;

typedef struct {
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
    int arr_delay;
    int soft_drop_factor;
    int gravity;
    int gravity_factor;

    const bool can_hold;
    const bool lock_on_down_held;
    const bool irs;
    const InstantDropType instant_drop_type;
    const int num_kicks;
    const Point (*const piece_rot_minos)[7][4][PIECE_MINO_COUNT];
    
    void (*const init)(void);
    Point (*const get_kick)(Rotation new_rotation, int attempt);
    void (*const on_lock)(bool cleared_lines);
    void (*const on_line_clear)(int num_lines);
    MinoType (*const generate_new_piece)(void);
    void (*const update)(void);
    void (*const draw)(void);

} Gamemode;

struct tagGameData {

    Piece piece;
    MovementData movement;
    Board board;
    Gamemode gamemode;

    bool input_held[NUM_HOLDABLE_KEYS];

    long timer_ms;

    int line_clear_timer;
    int are_timer;

    MinoType held_piece;
    bool has_held;

};

// game.c
void input_left(void);
void input_right(void);
void input_instant_drop(void);
void input_rotate_cw(void);
void input_rotate_ccw(void);
void input_hold(void);

void release_left(void);
void release_right(void);

void game_init(void);
const Point* get_piece_minos(void);
bool update(void);

// draw.c
void init_fonts();
void draw_mino_scaled(int cell_x, int cell_y, MinoType type, const Uint32 piece_colours[9], int scale);
void draw_mino(int cell_x, int cell_y, MinoType type, const Uint32 piece_colours[9]);
void draw_small_text(int x, int y, const char* text);
void draw_large_text(int x, int y, const char* text);
void draw_info_value(int row, const char* format, int value);
void draw_info_text(int row, const char* format, char* text);
void draw_info_timer(int row);

// mode.c
bool load_gamemode(int argc, char* argv[]);

// main.c
extern SDL_Renderer* renderer;
extern GameData state;

#endif