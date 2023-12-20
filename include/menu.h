#include "main.h"

#define MENU(...) { \
    .menu_items = (MenuItem[]) {__VA_ARGS__}, \
    .menu_item_count = sizeof((MenuItem[]) {__VA_ARGS__}) / sizeof(MenuItem) \
}

#define LABEL(label_text) { \
    .menu_item_type = MenuItemType_Label, \
    .menu_item = { \
        .label = label_text \
    } \
}

#define BUTTON_FUNC(button_text, callback_func) { \
    .menu_item_type = MenuItemType_Button, \
    .menu_item = { .button = { \
        .text = button_text, \
        .action_type = ActionType_Func, \
        .action = { \
            .func = callback_func \
        } \
    }} \
}

#define BUTTON_NEW_MENU(button_text, menu) { \
    .menu_item_type = MenuItemType_Button, \
    .menu_item = { .button = { \
        .text = button_text, \
        .action_type = ActionType_NewMenu, \
        .action = { \
            .new_menu = &menu \
        } \
    }} \
}

#define BUTTON_LOAD_GAMEMODE(button_text, gamemode) { \
    .menu_item_type = MenuItemType_Button, \
    .menu_item = { .button = { \
        .text = button_text, \
        .action_type = ActionType_LoadGamemode, \
        .action = { \
            .load_gamemode = &gamemode \
        } \
    }} \
}

#define NUM_BOX(num_box_title, num_box_min, num_box_max, val_ptr) { \
    .menu_item_type = MenuItemType_NumBox, \
    .menu_item = { .num_box = { \
        .title = num_box_title, \
        .min = num_box_min, \
        .max = num_box_max, \
        .val = &val_ptr \
    }} \
}

#define SEPARATOR { \
    .menu_item_type = MenuItemType_Separator \
}

struct Menu;
typedef struct Menu Menu;
typedef struct {
    char* text;
    enum {
        ActionType_Func,
        ActionType_NewMenu,
        ActionType_LoadGamemode
    } action_type;
    union {
        void (*const func)(void);
        const struct Menu* new_menu;
        const Gamemode* load_gamemode;
    } action;
} Button;

typedef struct {
    char* title;
    int min;
    int max;
    int* val;
} NumBox;

typedef enum {
    MenuItemType_Label,
    MenuItemType_Button,
    MenuItemType_NumBox,
    MenuItemType_Separator
} MenuItemType;

typedef struct {
    MenuItemType menu_item_type;
    union {
        char* label;
        Button button;
        NumBox num_box;
    } menu_item;
} MenuItem;

struct Menu {
    MenuItem* menu_items;
    unsigned int menu_item_count;
};

extern const Menu* current_menu;
extern int selected_menu_item;

// menu.c
extern const Menu main_menu;
extern Menu results_menu;
extern const Menu pause_menu;

void load_menu(const Menu* menu);

void menu_move_up(void);
void menu_move_down(void);
void menu_select(void);
void menu_handle_key(SDL_Scancode scancode);

void draw_menu(void);