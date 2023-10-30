/*
File:   game_view.h
Author: Taylor Robbins
Date:   09\11\2023
*/

#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

struct GameView_t
{
	v2 position;
	v2 targetPos;
	rec worldRec;
	reci worldReci;
	bool isMoving;
};

#endif //  _GAME_VIEW_H
