/*
File:   game_defines.h
Author: Taylor Robbins
Date:   10\27\2023
*/

#ifndef _GAME_DEFINES_H
#define _GAME_DEFINES_H

#define MAIN_FONT_PATH         "/System/Fonts/Asheville-Sans-14-Bold.pft"
#define BUTTON_FONT_PATH       "Resources/Fonts/pixel8"
#define ITEM_COUNT_FONT_PATH   "Resources/Fonts/blocky"
#define DEFAULT_WORLD_SIZE     NewVec2i(100, 100)
#define DEFAULT_WORLD_SEED     1
#define GAME_MAX_NUM_PARTICLES 300

#define FIRST_APP_STATE   AppState_MainMenu
#define RECIPE_BOOK_PATH  "Resources/Text/recipes.txt"
#define ITEM_BOOK_PATH    "Resources/Text/items.txt"

#define ITEM_ID_NONE     0
#define INVALID_FRAME    NewVec2i(-1, -1)

#define WORLD_GEN_DECOR_DENSITY 0.2f
#define WORLD_GEN_CANDY_DENSITY 0.08f

#define TILE_SIZE 24

#define MAX_NUM_COLLISIONS_PER_FRAME   8

#endif //  _GAME_DEFINES_H
