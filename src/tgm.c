#include "../include/main.h"

#define HISTORY_LEN 4
#define NUM_TRIES 4

const Point piece_rot_minos[8][4][PIECE_MINO_COUNT] = {
    {},
    {   // L
        {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{-1, -1}, { 0, -1}, { 0,  0}, { 0,  1}}
    },
    {   // J
        {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}},
        {{-1,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}}
    },
    {   // O
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}
    },
    {   // S
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}},
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    },
    {   // Z
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}}
    },
    {   // T
        {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0,  0}, {-1,  1}, { 0,  1}, { 1,  1}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, { 0,  1}}
    },
    {   // I
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}}
    }
};

const int spin_kicks[8][MAX_KICKS][2];

static MinoType history[HISTORY_LEN] = {Z};
static bool first_piece = true;

MinoType rand_piece() {

    if (first_piece) {
        return rand() % 4 + 4;

    } else {
        return rand() % 7 + 1;
    }
}

void tgm1_new_piece(GameData* state) {

    MinoType try_piece = rand_piece();

    for (int try = 0; try < NUM_TRIES; try++) {

        bool no_conflict = true;

        for (int i = 0; i < HISTORY_LEN; i++) {

            if (history[i] == try_piece) {
                no_conflict = false;
                break;
            }
        }

        if (no_conflict) {
            break;
        }

        try_piece = rand_piece();

    }

    for (int i = 1; i < HISTORY_LEN; i++) {
        history[i] = history[i - 1];
    }

    history[0] = try_piece;

    if (state->held_inputs[Input_Rot_CCW]) {
        state->piece.rotation = Rot_W;

    } else if (state->held_inputs[Input_Rot_CW]) {
        state->piece.rotation = Rot_E;
        
    } else {
        state->piece.rotation = Rot_N;
    }

    state->piece.x = 4;
    state->piece.y = INVISIBLE_ROWS;

    first_piece = false;

    state->piece.type = 1 + rand() % 7;
    memcpy(state->piece.minos, piece_rot_minos[state->piece.type][state->piece.rotation], PIECE_MINOS_BYTES);

}

Point tgm1_get_kick(GameData* state, Rotation new_rotation, int attempt) {

    if (state->piece.type == I) {
        return (Point){0, 0};
    }

    if (state->piece.type == L || state->piece.type == J || state->piece.type == T) {

        for (int dy = -1; dy < 2; dy++) {
            for (int dx = -1; dx < 2; dx++) {

                if (state->board.minos[state->piece.y + dy][state->piece.x + dx]) {

                    if (dx == 0) {
                        return (Point){0, 0};
                    }

                    goto no_centre_column;

                }
            }
        }
    }
    no_centre_column:
    
    if (attempt == 1){
        return (Point){1, 0};
    }
        
    return (Point){-1, 0};

}

const Gamemode tgm1_mode = {
    .line_clear_delay = 41,
    .are_delay = 30,
    .lock_delay = 30,
    .gravity_factor = 256,
    .das_delay = 16,
    .soft_drop_factor = 256,
    .gravity = 4,
    .can_hold = false,
    .num_kicks = 2,
    .piece_rot_minos = &piece_rot_minos,
    .new_piece = &tgm1_new_piece,
    .get_kick = &tgm1_get_kick
};