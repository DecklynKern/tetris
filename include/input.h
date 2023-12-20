#include <stdbool.h>
#include <SDL2/SDL_scancode.h>

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


void handle_keydown(SDL_Scancode scancode, bool repeat);
void handle_keyup(SDL_Scancode scancode);

extern const SDL_Scancode mapped_keys[NUM_HOLDABLE_KEYS];
extern bool input_held[NUM_HOLDABLE_KEYS];