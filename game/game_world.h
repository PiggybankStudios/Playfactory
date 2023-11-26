/*
File:   game_world.h
Author: Taylor Robbins
Date:   09\10\2023
*/

#ifndef _GAME_WORLD_H
#define _GAME_WORLD_H

struct WorldTile_t
{
	v2i pos;
	bool isSolid;
	u16 generatedId;
	u16 groundId;
	u16 placedId;
};

struct World_t
{
	MemArena_t* allocArena;
	v2i size;
	v2i pixelSize;
	u64 numTiles;
	WorldTile_t* tiles;
	u64 seed;
	RandomSeries_t genRand;
};

#endif //  _GAME_WORLD_H
