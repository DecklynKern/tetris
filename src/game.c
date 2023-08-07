#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>

#include "../include/main.h"

void input_left(GameData* state) {

    if (!state->line_clear_timer && !state->are_timer) {
        try_move(&state->board, &state->piece, -1, 0);
    }

    state->movement.das_timer = state->gamemode.das_delay;
    state->movement.das = false;
    state->movement.das_direction = -1;

}

void input_right(GameData* state) {

    if (!state->line_clear_timer && !state->are_timer) {
        try_move(&state->board, &state->piece, 1, 0);
    }

    state->movement.das_timer = state->gamemode.das_delay;
    state->movement.das = false;
    state->movement.das_direction = 1;

}

void input_down(GameData* state) {
    state->movement.down_held = true;
}

void input_rotate_cw(GameData* state) {
    if (!state->line_clear_timer && !state->are_timer) {
        try_rotate_piece(state, -1);
    }
}

void input_rotate_ccw(GameData* state) {
    if (!state->line_clear_timer && !state->are_timer) {
        try_rotate_piece(state, 1);
    }
}

void release_left(GameData* state) {
    if (state->movement.das_direction == -1 && (state->movement.das_timer || state->movement.das)) {
        state->movement.das_timer = 0;
        state->movement.das = 0;
        state->movement.das_direction = 0;
    }
}

void release_right(GameData* state) {
    if (state->movement.das_direction == 1 && (state->movement.das_timer || state->movement.das)) {
        state->movement.das_timer = 0;
        state->movement.das = 0;
        state->movement.das_direction = 0;
    }
}

void release_down(GameData* state) {
    state->movement.down_held = false;
}

bool update(GameData* state) {

    // technically missing das change restriction on frame when piece spawns
    if (
        state->movement.das_timer &&
        !state->line_clear_timer &&
        state->are_timer <= state->gamemode.are_delay - 4 &&
        state->are_timer != 1
    ) {

        state->movement.das_timer--;

        if (!state->movement.das_timer) {
            state->movement.das = 1;
        }
    }

    if (state->line_clear_timer) {

        state->line_clear_timer--;

        if (!state->line_clear_timer) {
            state->are_timer = state->gamemode.are_delay;
        }

        return false;

    }

    if (state->are_timer) {

        state->are_timer--;
    
        if (!state->are_timer) {

            (*state->gamemode.new_piece)(state);

            state->movement.lock_delay_timer = state->gamemode.lock_delay;
        
            if (!placement_valid(&state->board, state->piece.minos, state->piece.x, state->piece.y)) {
                return true;
            }
        }

        return false;

    }

    if (state->held_inputs[Input_Down]) {
        state->movement.gravity_count += state->gamemode.soft_drop_factor;
    }

    state->movement.gravity_count += state->gamemode.gravity_factor;

    while (state->movement.gravity_count > state->gamemode.gravity_factor) {

        state->movement.gravity_count -= state->gamemode.gravity_factor;

        if (!try_move(&state->board, &state->piece, 0, 1)) {
            break;
        }
    }

    if (state->movement.das) {
        try_move(&state->board, &state->piece, state->movement.das_direction, 0);
    }

    if (placement_valid(&state->board, state->piece.minos, state->piece.x, state->piece.y + 1)) {
        state->movement.lock_delay_timer = state->gamemode.lock_delay;

    } else {

        state->movement.gravity_count = 0;

        if (!state->movement.lock_delay_timer || state->held_inputs[Input_Down]) {
            
            if (lock_piece(&state->board, &state->piece)) {
                state->line_clear_timer = state->gamemode.line_clear_delay;

            } else {
                state->are_timer = state->gamemode.are_delay;
            }

        } else {
            state->movement.lock_delay_timer--;
        }
    }

    return false;

}