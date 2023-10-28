/*
File:   game_main.h
Author: Taylor Robbins
Date:   09\10\2023
*/

#ifndef _GAME_MAIN_H
#define _GAME_MAIN_H

#include "game_version.h"
#include "main_menu.h"
#include "game_state.h"
#include "game_main.h"

struct GameGlobals_t
{
	bool initialized;
	
	Font_t titleFont;
	RecipeBook_t recipeBook;
	ItemBook_t itemBook;
};

#endif //  _GAME_MAIN_H
