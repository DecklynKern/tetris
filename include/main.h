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
#define SCALE 24
#define ROW_BYTES BOARD_WIDTH * sizeof(MinoType)
#define MAX_KICKS 5

#define SMALL_FONT_SIZE 16
#define LARGE_FONT_SIZE 32

#define TOP_SPACE_HEIGHT 150
#define RIGHT_SPACE_WIDTH 150

#define SCREEN_WIDTH BOARD_WIDTH * SCALE + RIGHT_SPACE_WIDTH
#define SCREEN_HEIGHT (BOARD_HEIGHT - INVISIBLE_ROWS) * SCALE + TOP_SPACE_HEIGHT

#define PIECE_MINO_COUNT 4

#define RGB(r, g, b) (Uint32)((r) << 24 | (g) << 16 | (b) << 8 | 0xFF)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

typedef enum {
    Input_Left = 0,
    Input_Right,
    Input_Down,
    Input_InstantDrop,
    Input_Rot_CCW,
    Input_Rot_CW,
    Input_Hold,
    NUM_HOLDABLE_KEYS
} Inputs;

typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    Piece_Empty = 0,
    Piece_O,
    Piece_S,
    Piece_Z,
    Piece_L,
    Piece_J,
    Piece_T,
    Piece_I,
    Piece_Garbage
} MinoType;

typedef enum {
    Rot_N = 0,
    Rot_E,
    Rot_S,
    Rot_W
} Rotation;

typedef enum {
    Drop_NoInstant = 0,
    Drop_Sonic,
    Drop_Hard
} InstantDropType;

typedef enum {
    SOCD_None   = 0b00,
    SOCD_Left   = 0b01,
    SOCD_Right  = 0b10,
    SOCD_Both   = 0b11
} SOCD_Direction;

typedef struct {
    MinoType type;
    int x;
    int y;
    Rotation rotation;
} Piece;

typedef struct {
    MinoType minos[BOARD_HEIGHT][BOARD_WIDTH];
} Board;

struct GameData;
typedef struct GameData GameData;
typedef struct {
    
    const float fps;

    int line_clear_delay;
    int are_delay;
    int lock_delay;
    int das_delay;
    int arr_delay;
    int soft_drop_factor;
    int gravity;
    int gravity_factor;

    bool show_ghost;
    const bool can_hold;
    const bool lock_on_down_held;
    const bool irs;
    const InstantDropType instant_drop_type;
    const SOCD_Direction socd_allow_das_overwrite;
    const int num_kicks;
    const Point (*const piece_rot_minos)[7][4][PIECE_MINO_COUNT];
    const Uint32 (*piece_colours)[];
    
    void (*const init)(void);
    Point (*const get_kick)(Rotation new_rotation, int attempt);
    void (*const on_lock)(bool cleared_lines);
    void (*const on_line_clear)(int num_lines);
    MinoType (*const generate_new_piece)(void);
    void (*const update)(void);
    void (*const draw)(void);
    void (*const on_exit)(void);

} Gamemode;

struct GameData {

    Piece piece;
    
    struct {
        char line1[64];
        char line2[64];
        char line3[64];
    } result;
    
    Board board;
    Gamemode gamemode;
    const Gamemode* gamemode_ref;

    bool input_held[NUM_HOLDABLE_KEYS];

    bool quit_to_main_menu;
    long timer_ms;

    MinoType held_piece;
    bool has_held;

};

// game.c
bool board_is_clear(void);
bool piece_active(void);
bool placement_valid(const Point* piece_minos, int piece_x, int piece_y);

void input_left(void);
void input_right(void);
void input_down(void);
void input_instant_drop(void);
void input_rotate_cw(void);
void input_rotate_ccw(void);
void input_hold(void);

void release_left(void);
void release_right(void);
void release_down(void);

void game_init(void);
const Point* get_piece_minos(void);
void update(void);

// draw.c
void init_fonts();
void draw_mino_as_colour(int cell_x, int cell_y, int scale, Uint32 colour);
void draw_mino_scaled(int cell_x, int cell_y, MinoType type, int scale);
void draw_mino(int cell_x, int cell_y, MinoType type);
void draw_small_text(int x, int y, const char* text);
void draw_large_text(int x, int y, const char* text);
void draw_info_value(int row, const char* format, int value);
void draw_info_text(int row, const char* format, const char* text);
void draw_info_timer(int row);
void draw_piece_north(MinoType type, int offset_x, int offset_y);
void draw_board(void);
void draw_next(MinoType* next, int piece_count);

// timer.c
void timer_pause(void);
void timer_unpause(void);
double get_timer_seconds(void);
long get_timer_ms(void);
void get_timer_formatted(char* buf);

// main.c
extern SDL_Renderer* renderer;
extern GameData state;

#endif