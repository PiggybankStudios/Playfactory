/*
File:   game_defines.h
Author: Taylor Robbins
Date:   10\27\2023
*/

#ifndef _GAME_DEFINES_H
#define _GAME_DEFINES_H

#define MAIN_FONT_PATH         "/System/Fonts/Asheville-Sans-14-Bold.pft"
#define TITLE_FONT_PATH        "Resources/Fonts/SpaceContract"
#define MMENU_BTN_FONT_PATH    "Resources/Fonts/blocky"
#define BUTTON_FONT_PATH       "Resources/Fonts/pixel8"
#define ITEM_COUNT_FONT_PATH   "Resources/Fonts/blocky"
#define BEAN_COUNT_FONT_PATH   "Resources/Fonts/bean_font"
#define ITEM_NAME_FONT_PATH    "Resources/Fonts/whacky_joe"
#define DEFAULT_WORLD_SIZE     NewVec2i(100, 100)
#define DEFAULT_WORLD_SEED     1
#define GAME_MAX_NUM_PARTICLES 300

// #define FIRST_APP_STATE   AppState_MainMenu
#define FIRST_APP_STATE   AppState_None //TODO: Change me back!
#define RECIPE_BOOK_PATH  "Resources/Text/recipes.txt"
#define ITEM_BOOK_PATH    "Resources/Text/items.txt"

#define ITEM_ID_NONE     0
#define INVALID_FRAME    NewVec2i(-1, -1)

#define WORLD_GEN_DECOR_DENSITY 0.2f
#define WORLD_GEN_CANDY_DENSITY 0.08f

#define TILE_SIZE 24

#define MAX_NUM_COLLISIONS_PER_FRAME   8

#define GAME_VIEW_MOVE_LAG_WHILE_MOVING     8 //divisor
#define GAME_VIEW_MOVE_LAG_WHILE_STOPPED    6 //divisor
#define GAME_VIEW_STOP_MOVE_DIST_TO_TARGET  20 //px
#define GAME_VIEW_JUMP_TO_TARGET_DIST       1 //px
#define GAME_VIEW_SCREEN_EDGE_PADDING       25 //px
#define GAME_VIEW_VEL_BASED_LOOK_AHEAD_DIST NewVec2(0, 0) //NewVec2(100, 60) //px

#define PLAYDATE_LAUNCH_CARD_WIDTH 350
#define PLAYDATE_LAUNCH_CARD_HEIGHT 155
#define PLAYDATE_LAUNCH_CARD_SIZE NewVec2i(PLAYDATE_LAUNCH_CARD_WIDTH, PLAYDATE_LAUNCH_CARD_HEIGHT)
#define PLAYDATE_SCREEN_WIDTH 400
#define PLAYDATE_SCREEN_HEIGHT 240
#define PLAYDATE_SCREEN_SIZE NewVec2i(PLAYDATE_SCREEN_WIDTH, PLAYDATE_SCREEN_HEIGHT)

#endif //  _GAME_DEFINES_H
