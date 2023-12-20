#include "../include/input.h"
#include "../include/menu.h"

bool input_held[NUM_HOLDABLE_KEYS] = {0};

const SDL_Scancode mapped_keys[NUM_HOLDABLE_KEYS] = {
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_X,
    SDL_SCANCODE_C
};

static bool in_game(void) {
    return !current_menu;
}

void handle_keydown(SDL_Scancode scancode, bool repeat) {

    if (scancode == SDL_SCANCODE_ESCAPE) {
        
        if (current_menu == &pause_menu) {
            current_menu = NULL;
            timer_unpause();
        }
        else if (in_game()) {
            load_menu(&pause_menu);
            timer_pause();
        }

        return;

    }

    if (!in_game()) {
        
        switch (scancode) {
            case SDL_SCANCODE_UP:
                menu_move_up();
                break;
            case SDL_SCANCODE_DOWN:
                menu_move_down();
                break;
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_SPACE:
                menu_select();
                break;
            default:
                menu_handle_key(scancode);
        }
        
        return;
        
    }

    if (repeat) {
        return;
    }

    if (scancode == mapped_keys[Input_Left]) {
        input_left();
    }
    else if (scancode == mapped_keys[Input_Right]) {
        input_right();
    }
    else if (scancode == mapped_keys[Input_Down]) {
        input_down();
    }
    else if (scancode == mapped_keys[Input_InstantDrop]) {
        input_instant_drop();
    }
    else if (scancode == mapped_keys[Input_Rot_CCW]) {
        input_rotate_ccw();
    }
    else if (scancode == mapped_keys[Input_Rot_CW]) {
        input_rotate_cw();
    }
    else if (scancode == mapped_keys[Input_Hold]) {
        input_hold();
    }
}

void handle_keyup(SDL_Scancode scancode) {

    if (!in_game()) {
        return;
    }

    if (scancode == mapped_keys[Input_Left]) {
        release_left();
    }
    else if (scancode == mapped_keys[Input_Right]) {
        release_right();
    }
    else if (scancode == mapped_keys[Input_Down]) {
        release_down();
    }
}