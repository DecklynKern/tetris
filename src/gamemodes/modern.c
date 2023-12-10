#include "../../include/main.h"

static const Uint32 piece_colours[9] = {
    RGB(  0,   0,   0),
    RGB(255, 255,   0),
    RGB(  0, 255,   0),
    RGB(255,   0,   0),
    RGB(255, 128,   0),
    RGB(  0,   0, 255),
    RGB(255,   0, 255),
    RGB(  0, 255, 255),
    RGB(200, 200, 200)
};

static const Point srs_minos[7][4][PIECE_MINO_COUNT] = {
    { // O
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
        {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}
    },
    { // S
        {{ 0, -1}, { 1, -1}, {-1,  0}, { 0,  0}},
        {{-1, -1}, {-1,  0}, { 0,  0}, { 0,  1}},
        {{ 0,  0}, { 1,  0}, {-1,  1}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}}
    },
    { // Z
        {{-1, -1}, { 0, -1}, { 0,  0}, { 1,  0}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, {-1,  1}},
        {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}},
        {{ 1, -1}, { 0,  0}, { 1,  0}, { 0,  1}}
    }, { // L
        {{ 1, -1}, {-1,  0}, { 0,  0}, { 1,  0}},
        {{-1, -1}, { 0, -1}, { 0,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}}
    },
    { // J
        {{-1, -1}, {-1,  0}, { 0,  0}, { 1,  0}},
        {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}},
        {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}}
    },
    { // T
        {{ 0, -1}, {-1,  0}, { 0,  0}, { 1,  0}},
        {{ 0, -1}, {-1,  0}, { 0,  0}, { 0,  1}},
        {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}},
        {{ 0, -1}, { 0,  0}, { 1,  0}, { 0,  1}}
    },
    { // I
        {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}},
        {{ 0, -1}, { 0,  0}, { 0,  1}, { 0,  2}},
        {{-1,  1}, { 0,  1}, { 1,  1}, { 2,  1}},
        {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}}
    }
};

#define KICK_TRIES 4
// NESW/0R2L
static const Point srs_szljt_kicks[4][4][KICK_TRIES] = {
    {
        {},
        {{-1,  0}, {-1, -1}, { 0,  2}, {-1,  2}},
        {},
        {{ 1,  0}, { 1, -1}, { 0,  2}, { 1,  2}}
    },
    {
        {{-1,  0}, {-1,  1}, { 0, -2}, {-1, -2}},
        {},
        {{-1,  0}, {-1,  1}, { 0, -2}, {-1, -2}},
        {}
    },
    {
        {},
        {{-1,  0}, {-1, -1}, { 0,  2}, {-1,  2}},
        {},
        {{ 1,  0}, { 1, -1}, { 0,  2}, { 1,  2}}
    },
    {
        {{ 1,  0}, { 1,  1}, { 0, -2}, { 1, -2}},
        {},
        {{ 1,  0}, { 1,  1}, { 0, -2}, { 1, -2}},
        {}
    }
};

static const Point srs_i_kicks[4][4][KICK_TRIES] = {
    {
        {},
        {{-2,  0}, { 1,  0}, {-2,  1}, { 1, -2}},
        {},
        {{-1,  0}, { 2,  0}, {-1, -2}, { 2,  1}}
    },
    {
        {{ 1,  0}, {-2,  0}, { 1,  2}, {-2, -1}},
        {},
        {{-2,  0}, {-1,  0}, {-2,  1}, { 1, -2}},
        {}
    },
    {
        {},
        {{ 1,  0}, {-2,  0}, { 1,  2}, {-2, -1}},
        {},
        {{ 2,  0}, {-1,  0}, { 2, -1}, {-1,  2}}
    },
    {
        {{ 2,  0}, {-1,  0}, { 2, -1}, {-1,  2}},
        {},
        {{-1,  0}, { 2,  0}, {-1, -2}, { 2,  1}},
        {}
    }
};

static MinoType bag[7];

static void new_bag(void) {

    bag[0] = O;
    bag[1] = S;
    bag[2] = Z;
    bag[3] = L;
    bag[4] = J;
    bag[5] = T;
    bag[6] = I;

    // https://stackoverflow.com/a/6127606
    for (int i = 0; i < 7; i++) {

        int j = i + rand() / (RAND_MAX / (7 - i) + 1);

        int temp = bag[j];
        bag[j] = bag[i];
        bag[i] = temp;

    }
}

static void init(void) {
    new_bag();
}

static Point get_kick(Rotation new_rotation, int attempt) {

    if (state.piece.type == O) {
        return (Point){0, 0};
    }

    return (state.piece.type == I ? srs_i_kicks : srs_szljt_kicks)[state.piece.rotation][new_rotation][attempt - 1];

}

static void on_line_clear(int num_lines) {
}

static MinoType generate_new_piece(void) {

    MinoType new_piece = bag[0];

    if (bag[1] == Empty) {
        new_bag();
    }
    else {
        
        for (int i = 0; i < 6; i++) {
            bag[i] = bag[i + 1];
        }

        bag[6] = Empty;

    }

    return new_piece;

}

static void draw(void) {
    draw_board();
    draw_single_next(bag[0]);
}

const Gamemode modern_mode = {

    .line_clear_delay = 35,
    .are_delay = 7,
    .lock_delay = 30,
    .das_delay = 10,
    .arr_delay = 2,
    .soft_drop_factor = 256,
    .gravity = 4,
    .gravity_factor = 256,

    .show_ghost = true,
    .can_hold = true,
    .lock_on_down_held = false,
    .irs = false,
    .instant_drop_type = Drop_Hard,
    .socd_allow_das_overwrite = SOCD_Both,
    .num_kicks = 4,
    .piece_rot_minos = &srs_minos,
    .piece_colours = (Uint32 (*const)[]) &piece_colours,

    .init = init,
    .get_kick = get_kick,
    .on_line_clear = on_line_clear,
    .generate_new_piece = generate_new_piece,
    .draw = draw

};