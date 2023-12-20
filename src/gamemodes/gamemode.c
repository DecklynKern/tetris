#include "../../include/menu.h"

void load_gamemode(const Gamemode *gamemode) {
    
    timer_reset();

    memcpy(&state.gamemode, gamemode, sizeof(Gamemode));
    state.gamemode_ref = gamemode;
    
    board_clear();
    state.gamemode.init();
    
    game_init();
    load_menu(&pause_menu);
    
            
}