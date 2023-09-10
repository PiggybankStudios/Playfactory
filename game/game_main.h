/*
File:   game_main.h
Author: Taylor Robbins
Date:   09\10\2023
*/

#ifndef _GAME_MAIN_H
#define _GAME_MAIN_H

#define FIRST_APP_STATE AppState_Game

struct GameGlobals_t
{
	bool initialized;
	
	Font_t titleFont;
};

#endif //  _GAME_MAIN_H
