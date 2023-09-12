/*
File:   game_view.h
Author: Taylor Robbins
Date:   09\11\2023
*/

#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

#define GAME_VIEW_MOVE_LAG_WHILE_MOVING     10 //divisor
#define GAME_VIEW_MOVE_LAG_WHILE_STOPPED    6 //divisor
#define GAME_VIEW_STOP_MOVE_DIST_TO_TARGET  20 //px
#define GAME_VIEW_JUMP_TO_TARGET_DIST       1 //px
#define GAME_VIEW_SCREEN_EDGE_PADDING       25 //px
#define GAME_VIEW_VEL_BASED_LOOK_AHEAD_DIST NewVec2(100, 60) //px

struct GameView_t
{
	v2 position;
	v2 targetPos;
	rec worldRec;
	reci worldReci;
	bool isMoving;
};

#endif //  _GAME_VIEW_H
