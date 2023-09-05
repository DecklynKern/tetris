#include <string.h>

#include "../../include/gamemodes.h"

const char* gamemode_names[NUM_GAMEMODES] = {
    "NES",
    "TGM1",
    "TAP Master",
    "TAP TGM+",
    "TAP Death",
    "Modern"
};

static const Gamemode* gamemodes[NUM_GAMEMODES] = {
    &nes_mode,
    &tgm1_mode,
    &tap_master_mode,
    &tap_tgmplus_mode,
    &tap_death_mode,
    &modern_mode
};

void load_gamemode(int gamemode) {
    memcpy(&state.gamemode, gamemodes[gamemode], sizeof(Gamemode));
}