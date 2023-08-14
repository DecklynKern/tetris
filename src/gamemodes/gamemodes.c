#include <string.h>

#include "../../include/gamemodes.h"

#define NUM_GAMEMODES 6
static const struct {
    char* code;
    const Gamemode* mode;
} gamemodes[NUM_GAMEMODES] = {
    {"nes", &nes_mode},
    {"tgm1", &tgm1_mode},
    {"tap-master", &tap_master_mode},
    {"tap-tgm+", &tap_tgmplus_mode},
    {"tap-death", &tap_death_mode},
    {"modern", &modern_mode},
};

bool load_gamemode(int argc, char* argv[]) {

    if (!strcmp(argv[1], "list")) {
        
        printf("Available modes:\n");
        
        for (int i = 0; i < NUM_GAMEMODES; i++) {
            printf("%s\n", gamemodes[i].code);
        }

        return false;

    }

    for (int i = 0; i < NUM_GAMEMODES; i++) {
        if (!strcmp(argv[1], gamemodes[i].code)) {
            memcpy(&state.gamemode, gamemodes[i].mode, sizeof(Gamemode));
            return true;
        }
    }

    printf("Invalid gamemode, use './tetrism list' to see all supported modes.\n");
    return false;

}