#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../../include/gamemodes.h"

const Menu* current_menu = NULL;
int selected_menu_item = 0;

const Menu main_menu = {
    .menu_items = (MenuItem[]) {
        LABEL("Select Game"),
        SEPARATOR,
        BUTTON_NEW_MENU("NES", nes_menu),
        BUTTON_NEW_MENU("TGM", tgm_menu),
        BUTTON_LOAD_GAMEMODE("Modern", modern_mode)
    },
    .menu_item_count = 5
};

static void unpause(void) {
    current_menu = NULL;
    timer_unpause();
}

const Menu pause_menu = {
    .menu_items = (MenuItem[]) {
        LABEL("PAUSED"),
        SEPARATOR,
        BUTTON_FUNC("Resume", unpause),
        BUTTON_NEW_MENU("Main Menu", main_menu)
    },
    .menu_item_count = 4
};

static bool item_selectable(MenuItemType type) {
    switch (type) {
        case MenuItemType_Label:
        case MenuItemType_Separator:
            return false;
        default:
            return true;
    }
}

void load_menu(const Menu *menu) {
    
    for (int i = 0; i < menu->menu_item_count; i++) {
        if (item_selectable(current_menu->menu_items[i].menu_item_type)) {
            selected_menu_item = i;
            break;
        }
    }
    
    current_menu = menu;
    
}

void menu_move_up(void) {
    
    for (int i = selected_menu_item - 1; i >= 0; i--) {
        if (item_selectable(current_menu->menu_items[i].menu_item_type)) {
            selected_menu_item = i;
            break;
        }
    }
}

void menu_move_down(void) {
    
    for (int i = selected_menu_item + 1; i < current_menu->menu_item_count; i++) {
        if (item_selectable(current_menu->menu_items[i].menu_item_type)) {
            selected_menu_item = i;
            break;
        }
    }
}

void button_press(Button* button) {
    
    switch (button->action_type) {
        case ActionType_Func:
            (button->action.func)();
            break;
        case ActionType_NewMenu:
            load_menu(button->action.new_menu);
            break;
        case ActionType_LoadGamemode:
            memcpy(&state.gamemode, button->action.load_gamemode, sizeof(Gamemode));
            state.gamemode.init();
            game_init();
            load_menu(&pause_menu);
            break;
    }
}

void menu_select(void) {
    
    switch (current_menu->menu_items[selected_menu_item].menu_item_type) {
        case MenuItemType_Button:
            button_press(&current_menu->menu_items[selected_menu_item].menu_item.button);
            break;
        default:
            break;
    }
}

void menu_handle_key(SDL_Scancode scancode) {
    
    MenuItem* menu_item = &current_menu->menu_items[selected_menu_item];
    
    switch (menu_item->menu_item_type) {
        case MenuItemType_NumBox:
            
            if (scancode == SDL_SCANCODE_MINUS) {
                
                if (*menu_item->menu_item.num_box.val > menu_item->menu_item.num_box.min) {
                    (*menu_item->menu_item.num_box.val)--;
                }
            }
            else if (scancode == SDL_SCANCODE_EQUALS) {
                
                if (*menu_item->menu_item.num_box.val < menu_item->menu_item.num_box.max) {
                    (*menu_item->menu_item.num_box.val)++;
                }
            }
            
            break;

        default:
            break;
            
    }
}

void draw_menu() {
    
    int y = 0;
    
    for (int i = 0; i < current_menu->menu_item_count; i++) {
        
        y += LARGE_FONT_SIZE;
        
        if (selected_menu_item == i) {
            
            SDL_Rect rect = {
                0,
                y,
                SCREEN_WIDTH,
                LARGE_FONT_SIZE
            };
            
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &rect);
            
        }
        
        switch (current_menu->menu_items[i].menu_item_type) {
            case MenuItemType_Label:            
                draw_large_text(SCALE * 5, y, current_menu->menu_items[i].menu_item.label);
                break;
            case MenuItemType_Button:
                draw_large_text(SCALE * 5, y, current_menu->menu_items[i].menu_item.button.text);
                break;
            case MenuItemType_NumBox:
            {
                
                char num_text[10];
                sprintf(num_text, "%d", *current_menu->menu_items[i].menu_item.num_box.val);
                
                draw_large_text(SCALE, y, current_menu->menu_items[i].menu_item.num_box.title);
                draw_large_text(SCALE * 8, y, num_text);
                
                break;
                
            }
            case MenuItemType_Separator:
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                SDL_RenderDrawLine(renderer, 0, y + LARGE_FONT_SIZE / 2, SCREEN_WIDTH, y + LARGE_FONT_SIZE / 2);
                break;
        }
    }
}