/*
File:   game_world.h
Author: Taylor Robbins
Date:   09\10\2023
*/

#ifndef _GAME_WORLD_H
#define _GAME_WORLD_H

#define WORLD_GEN_GRASS_DENSITY 0.2f
#define WORLD_GEN_CANDY_DENSITY 0.08f

#define TILE_SIZE 24

#define MAX_NUM_COLLISIONS_PER_FRAME   8

struct WorldTile_t
{
	TileType_t type;
	v2i pos;
	bool isSolid;
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
