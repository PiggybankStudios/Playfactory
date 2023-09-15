/*
File:   game_player.h
Author: Taylor Robbins
Date:   09\10\2023
*/

#ifndef _GAME_PLAYER_H
#define _GAME_PLAYER_H

#define PLAYER_SIZE          NewVec2i(32, 32)
#define PLAYER_COLLISION_REC NewRec(-7, 0, 14, 14)
#define PLAYER_TARGET_OFFSET NewVec2(0, 2)
#define PLAYER_TARGET_DIST   24
#define PLAYER_MINING_SPEED  (1.0f / 360.0f) //progress/degree

#define PLAYER_WALK_SPEED             6.0f
#define PLAYER_RUN_SPEED              10.0f
#define PLAYER_DOUBLE_TAP_RUN_TIMEOUT 250 //ms
#define PLAYER_STOP_RUN_TIMEOUT       200 //ms
#define PLAYER_FRICTION               50 //percent

struct Player_t
{
	MemArena_t* allocArena;
	v2 position;
	rec colRec;
	v2 velocity;
	v2 lookVec;
	Dir2Ex_t inputDir;
	bool isRunning;
	u64 lastMoveTime;
	Dir2Ex_t rotation;
	u64 lastArrowPressTime[Dir2_Count];
	
	Inventory_t inventory;
	Inventory_t scienceInventory;
	
	bool isMining;
	r32 miningProgress;
	v2 targetPos;
	v2i targetTilePos;
	WorldTile_t* targetTile;
};

#endif //  _GAME_PLAYER_H
