#include "../../include/menu.h"

void load_gamemode(const Gamemode *gamemode) {
    
    memcpy(&state.gamemode, gamemode, sizeof(Gamemode));
    state.gamemode_ref = gamemode;
    
    state.gamemode.init();
    
    game_init();
    load_menu(&pause_menu);
            
}