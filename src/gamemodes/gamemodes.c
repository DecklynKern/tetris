#include <string.h>

#include "../../include/gamemodes.h"

bool load_gamemode(int argc, char* argv[]) {

    const Gamemode* mode = NULL;

    if (!strcmp(argv[1], "tgm1")) {
        mode = &tgm1_mode;
    
    } else if (!strcmp(argv[1], "nes")) {
        mode = &nes_mode;

    } else if (!strcmp(argv[1], "tap-death")) {
        mode = &tap_death_mode;
    }

    memcpy(&state.gamemode, mode, sizeof(Gamemode));

    return mode != NULL;

}